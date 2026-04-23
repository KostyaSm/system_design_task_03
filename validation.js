const db = db.getSiblingDB('fitness_tracker');

db.createCollection("workouts", {
  validator: {
    $jsonSchema: {
      bsonType: "object",
      required: ["id", "user_id", "date", "exercises"],
      properties: {
        id: { bsonType: "int" },
        user_id: { bsonType: "int" },
        title: { bsonType: "string", minLength: 1, maxLength: 100 },
        date: { bsonType: "date" },
        duration_seconds: { bsonType: ["int", "null"], minimum: 0, maximum: 28800 },
        exercises: {
          bsonType: "array",
          minItems: 1,
          items: {
            bsonType: "object",
            required: ["exercise_id", "exercise_name", "order"],
            properties: {
              exercise_id: { bsonType: "int" },
              exercise_name: { bsonType: "string", minLength: 1, maxLength: 100 },
              sets: { bsonType: ["int", "null"], minimum: 1, maximum: 50 },
              reps: { bsonType: ["int", "null"], minimum: 1, maximum: 200 },
              weight_kg: { bsonType: ["double", "int", "null"], minimum: 0, maximum: 1000 },
              duration_seconds: { bsonType: ["int", "null"], minimum: 0, maximum: 7200 },
              order: { bsonType: "int", minimum: 0 }
            }
          }
        }
      }
    }
  },
  validationLevel: "strict",
  validationAction: "error"
});

print("Создана");

try {
  db.workouts.insertOne({
    id: 999,
    user_id: 1,
    exercises: [{ exercise_id: 1, exercise_name: "Тест", sets: 1, reps: 1, order: 0 }]
  });
  print("Валидация не сработала");
} catch (e) {
  print("+");
}

try {
  db.workouts.insertOne({
    id: 998,
    user_id: 1,
    date: new Date(),
    exercises: [{ exercise_id: 1, exercise_name: "Тест", sets: 1, reps: 1, weight_kg: -50, order: 0 }]
  });
  print("-");
} catch (e) {
  print("Успез");
}