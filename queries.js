const db = db.getSiblingDB('fitness_tracker');

print("\n === 1. CREATE (Создание документов) ===");

db.users.insertOne({
  id: 11,
  login: "test_user",
  first_name: "Тест",
  last_name: "Пользователь",
  email: "test@fit.ru",
  password_hash: "pw$test123",
  created_at: new Date(),
  settings: { unit_system: "metric", privacy: "public" }
});


db.exercises.insertOne({
  id: 11,
  name: "Тяга гантели в наклоне",
  category: "strength",
  muscle_groups: ["back", "biceps"],
  difficulty: "intermediate",
  created_by: 1,
  is_public: true,
  created_at: new Date()
});


db.workouts.insertOne({
  id: 11,
  user_id: 1,
  title: "Тестовая тренировка",
  date: new Date(),
  duration_seconds: 1800,
  exercises: [
    { exercise_id: 11, exercise_name: "Тяга гантели в наклоне", sets: 3, reps: 10, weight_kg: 18, order: 0 }
  ],
  created_at: new Date()
});

print(" Созданы: пользователь, упражнение, тренировка");

print("\n🔵 === 2. READ (Чтение и поиск) ===");
   
const userByLogin = db.users.findOne({ login: { $eq: "ivanov" } });
print(`👤 Найден пользователь: ${userByLogin.first_name} ${userByLogin.last_name}`);



const usersByMask = db.users.find({
  $or: [
    { first_name: { $regex: "ан", $options: "i" } },
    { last_name: { $regex: "ан", $options: "i" } }
  ]
}).toArray();
print(`Найдено по маске "ан": ${usersByMask.length} пользователей`);
 
const strengthExercises = db.exercises.find({
  $and: [
    { category: "strength" },
    { difficulty: { $in: ["beginner", "intermediate"] } },
    { is_public: { $ne: false } }
  ]
}).toArray();
print(` Силовых упражнений (начинающие/средние): ${strengthExercises.length}`);


const workoutHistory = db.workouts.find({
  user_id: 1,
  date: { $gte: new Date("2024-01-01"), $lt: new Date("2025-01-01") }
}).sort({ date: -1 }).limit(5).toArray();
print(`История тренировок пользователя 1 за 2024: ${workoutHistory.length} записей`);

print("\n === 3. UPDATE (Обновление документов) ===");

db.users.updateOne(
  { login: "ivanov" },
  { $set: { last_name: "Иванов-Обновленный", settings: { unit_system: "metric", privacy: "private" } } }
);
print(" Профиль пользователя обновлен");
 
db.workouts.updateOne(
  { id: 1 },
  { $push: { exercises: { exercise_id: 8, exercise_name: "Отжимания", sets: 3, reps: 15, weight_kg: 0, order: 2 } } }
);
print(" Упражнение добавлено");

db.workouts.updateOne(
  { id: 1, "exercises.exercise_id": 1 },
  { $set: { "exercises.$.weight_kg": 85, "exercises.$.sets": 5 } }
);
print(" Параметры жима в тренировке #1 обновлены");


db.workouts.updateOne(
  { id: 1 },
  { $addToSet: { tags: "chest_day", tags: "progress" } }
);
print("Теги добавлены");

print("\n === 4. DELETE (Удаление документов) ==="); 

db.workouts.updateOne(
  { id: 1 },
  { $pull: { exercises: { exercise_id: 4 } } }
);
print("Подтягивания удалены");

 
const targetUserId = 11;
db.workouts.deleteMany({ user_id: targetUserId });
db.users.deleteOne({ id: targetUserId });
print(`Пользователь ${targetUserId} и его тренировки удалены`);

print("\n === 5. AGGREGATION (Статистика за период) ===");




const stats = db.workouts.aggregate([
  { $match: {
      user_id: 1,
      date: { $gte: new Date("2024-01-01"), $lt: new Date("2025-01-01") }
  }},
  { $unwind: "$exercises" },
  { $match: { "exercises.weight_kg": { $gte: 0 } } }, // исключаем кардио/null
  { $group: {
      _id: "$exercises.exercise_id",
      exercise_name: { $first: "$exercises.exercise_name" },
      total_volume_kg: { $sum: { $multiply: ["$exercises.sets", "$exercises.reps", "$exercises.weight_kg"] } },
      total_sets: { $sum: "$exercises.sets" },
      max_weight: { $max: "$exercises.weight_kg" }
  }},
  { $project: {
      _id: 0,
      exercise_id: "$_id",
      exercise_name: 1,
      total_volume_kg: 1,
      avg_weight_per_set: { $round: [{ $divide: ["$total_volume_kg", "$total_sets"] }, 2] }
  }},
  { $sort: { total_volume_kg: -1 } }
]).toArray();

print("\nСтатистика по объему (кг) за 2024 год:");
stats.forEach(s => print(`   • ${s.exercise_name}: ${s.total_volume_kg} кг (ср. вес: ${s.avg_weight_per_set} кг)`));

print("\nВсе запросы выполнены");