const db = db.getSiblingDB('fitness_tracker');


const users = [
  { id: 1, login: "ivanov", first_name: "Иван", last_name: "Иванов", email: "ivanov@mail.ru", password_hash: "pw$test123", created_at: new Date("2023-01-15"), settings: { unit_system: "metric", privacy: "public" } },
  { id: 2, login: "petrova", first_name: "Анна", last_name: "Петрова", email: "petrova@mail.ru", password_hash: "pw$test123", created_at: new Date("2023-02-20"), settings: { unit_system: "metric", privacy: "private" } },
  { id: 3, login: "sidorov", first_name: "Петр", last_name: "Сидоров", email: "sidorov@mail.ru", password_hash: "pw$test123", created_at: new Date("2023-03-10"), settings: { unit_system: "imperial", privacy: "public" } },
  { id: 4, login: "kuznetsova", first_name: "Мария", last_name: "Кузнецова", email: null, password_hash: "pw$test123", created_at: new Date("2023-04-05"), settings: { unit_system: "metric", privacy: "public" } },
  { id: 5, login: "smirnov", first_name: "Алексей", last_name: "Смирнов", email: "smirnov@mail.ru", password_hash: "pw$test123", created_at: new Date("2023-05-12"), settings: { unit_system: "metric", privacy: "private" } },
  { id: 6, login: "volkova", first_name: "Елена", last_name: "Волкова", email: "volkova@mail.ru", password_hash: "pw$test123", created_at: new Date("2023-06-18"), settings: { unit_system: "metric", privacy: "public" } },
  { id: 7, login: "novikov", first_name: "Дмитрий", last_name: "Новиков", email: "novikov@mail.ru", password_hash: "pw$test123", created_at: new Date("2023-07-22"), settings: { unit_system: "imperial", privacy: "public" } },
  { id: 8, login: "morozova", first_name: "Ольга", last_name: "Морозова", email: null, password_hash: "pw$test123", created_at: new Date("2023-08-30"), settings: { unit_system: "metric", privacy: "private" } },
  { id: 9, login: "vasiliev", first_name: "Сергей", last_name: "Васильев", email: "vasiliev@mail.ru", password_hash: "pw$test123", created_at: new Date("2023-09-14"), settings: { unit_system: "metric", privacy: "public" } },
  { id: 10, login: "pavlova", first_name: "Наталья", last_name: "Павлова", email: "pavlova@mail.ru", password_hash: "pw$test123", created_at: new Date("2023-10-01"), settings: { unit_system: "metric", privacy: "public" } }
];


const exercises = [
  { id: 1, name: "Жим штанги лежа", description: "Базовое упражнение на грудь", category: "strength", muscle_groups: ["chest", "triceps", "shoulders"], difficulty: "intermediate", created_by: 1, created_at: new Date("2023-01-10"), is_public: true },
  { id: 2, name: "Приседания со штангой", description: "Базовое упражнение на ноги", category: "strength", muscle_groups: ["quads", "glutes", "hamstrings"], difficulty: "intermediate", created_by: 1, created_at: new Date("2023-01-10"), is_public: true },
  { id: 3, name: "Становая тяга", description: "Базовое упражнение на спину и ноги", category: "strength", muscle_groups: ["back", "glutes", "hamstrings"], difficulty: "advanced", created_by: 2, created_at: new Date("2023-01-12"), is_public: true },
  { id: 4, name: "Подтягивания", description: "Упражнение на спину и бицепс", category: "strength", muscle_groups: ["back", "biceps"], difficulty: "intermediate", created_by: 3, created_at: new Date("2023-01-15"), is_public: true },
  { id: 5, name: "Бег 5 км", description: "Кардио на выносливость", category: "cardio", muscle_groups: ["cardio", "legs"], difficulty: "beginner", created_by: 4, created_at: new Date("2023-02-01"), is_public: true },
  { id: 6, name: "Планка", description: "Статическое упражнение на кор", category: "flexibility", muscle_groups: ["core"], difficulty: "beginner", created_by: 5, created_at: new Date("2023-02-05"), is_public: true },
  { id: 7, name: "Берпи", description: "Высокоинтенсивное интервальное упражнение", category: "hiit", muscle_groups: ["full_body"], difficulty: "advanced", created_by: 6, created_at: new Date("2023-02-10"), is_public: true },
  { id: 8, name: "Отжимания", description: "Упражнение на грудные и трицепс", category: "strength", muscle_groups: ["chest", "triceps"], difficulty: "beginner", created_by: 7, created_at: new Date("2023-02-15"), is_public: true },
  { id: 9, name: "Выпады с гантелями", description: "Упражнение на ноги", category: "strength", muscle_groups: ["quads", "glutes"], difficulty: "intermediate", created_by: 8, created_at: new Date("2023-02-20"), is_public: true },
  { id: 10, name: "Скакалка 15 мин", description: "Кардио для разминки", category: "cardio", muscle_groups: ["cardio", "calves"], difficulty: "beginner", created_by: 9, created_at: new Date("2023-02-25"), is_public: true }
];


const workouts = [
  { id: 1, user_id: 1, title: "Грудь + Трицепс", date: new Date("2024-01-10T18:00:00Z"), duration_seconds: 3600, notes: "Отличная тренировка", exercises: [{ exercise_id: 1, exercise_name: "Жим штанги лежа", sets: 4, reps: 10, weight_kg: 80, order: 0 }, { exercise_id: 4, exercise_name: "Подтягивания", sets: 3, reps: 8, weight_kg: 0, order: 1 }], created_at: new Date() },
  { id: 2, user_id: 1, title: "Ноги", date: new Date("2024-01-13T17:30:00Z"), duration_seconds: 3200, notes: "", exercises: [{ exercise_id: 2, exercise_name: "Приседания со штангой", sets: 4, reps: 12, weight_kg: 100, order: 0 }, { exercise_id: 9, exercise_name: "Выпады с гантелями", sets: 3, reps: 10, weight_kg: 24, order: 1 }], created_at: new Date() },
  { id: 3, user_id: 2, title: "Кардио + Пресс", date: new Date("2024-01-11T07:00:00Z"), duration_seconds: 2700, notes: "Утренняя пробежка", exercises: [{ exercise_id: 5, exercise_name: "Бег 5 км", sets: 1, reps: null, weight_kg: null, duration_seconds: 1800, order: 0 }, { exercise_id: 6, exercise_name: "Планка", sets: 3, reps: null, weight_kg: null, duration_seconds: 60, order: 1 }], created_at: new Date() },
  { id: 4, user_id: 2, title: "Верх тела", date: new Date("2024-01-14T19:00:00Z"), duration_seconds: 3000, notes: "", exercises: [{ exercise_id: 1, exercise_name: "Жим штанги лежа", sets: 3, reps: 10, weight_kg: 50, order: 0 }, { exercise_id: 4, exercise_name: "Подтягивания", sets: 3, reps: 6, weight_kg: 0, order: 1 }], created_at: new Date() },
  { id: 5, user_id: 3, title: "HIIT", date: new Date("2024-01-12T16:00:00Z"), duration_seconds: 1800, notes: "Очень тяжело", exercises: [{ exercise_id: 7, exercise_name: "Берпи", sets: 5, reps: 10, weight_kg: null, order: 0 }, { exercise_id: 8, exercise_name: "Отжимания", sets: 4, reps: 20, weight_kg: null, order: 1 }], created_at: new Date() },
  { id: 6, user_id: 3, title: "Спина", date: new Date("2024-01-15T18:30:00Z"), duration_seconds: 3400, notes: "", exercises: [{ exercise_id: 3, exercise_name: "Становая тяга", sets: 4, reps: 8, weight_kg: 120, order: 0 }, { exercise_id: 4, exercise_name: "Подтягивания", sets: 4, reps: 8, weight_kg: 0, order: 1 }], created_at: new Date() },
  { id: 7, user_id: 4, title: "Фулбоди", date: new Date("2024-01-16T08:00:00Z"), duration_seconds: 2800, notes: "", exercises: [{ exercise_id: 2, exercise_name: "Приседания со штангой", sets: 3, reps: 10, weight_kg: 70, order: 0 }, { exercise_id: 1, exercise_name: "Жим штанги лежа", sets: 3, reps: 10, weight_kg: 60, order: 1 }, { exercise_id: 4, exercise_name: "Подтягивания", sets: 3, reps: 5, weight_kg: 0, order: 2 }], created_at: new Date() },
  { id: 8, user_id: 5, title: "Кардио", date: new Date("2024-01-17T07:30:00Z"), duration_seconds: 2400, notes: "", exercises: [{ exercise_id: 10, exercise_name: "Скакалка 15 мин", sets: 1, reps: null, weight_kg: null, duration_seconds: 900, order: 0 }, { exercise_id: 5, exercise_name: "Бег 5 км", sets: 1, reps: null, weight_kg: null, duration_seconds: 1500, order: 1 }], created_at: new Date() },
  { id: 9, user_id: 6, title: "Сила", date: new Date("2024-01-18T19:00:00Z"), duration_seconds: 4000, notes: "Пробовал новый вес", exercises: [{ exercise_id: 3, exercise_name: "Становая тяга", sets: 5, reps: 5, weight_kg: 140, order: 0 }, { exercise_id: 2, exercise_name: "Приседания со штангой", sets: 4, reps: 8, weight_kg: 110, order: 1 }], created_at: new Date() },
  { id: 10, user_id: 7, title: "Разминка + Кор", date: new Date("2024-01-19T06:30:00Z"), duration_seconds: 1500, notes: "", exercises: [{ exercise_id: 10, exercise_name: "Скакалка 15 мин", sets: 1, reps: null, weight_kg: null, duration_seconds: 600, order: 0 }, { exercise_id: 6, exercise_name: "Планка", sets: 3, reps: null, weight_kg: null, duration_seconds: 90, order: 1 }], created_at: new Date() }
];


const counters = [
  { _id: "user_id", seq: 10 },
  { _id: "exercise_id", seq: 10 },
  { _id: "workout_id", seq: 10 }
];


db.users.insertMany(users);
db.exercises.insertMany(exercises);
db.workouts.insertMany(workouts);
db.counters.insertMany(counters);

print(`Загружено: ${users.length} пользователей, ${exercises.length} упражнений, ${workouts.length} тренировок`);