const db = db.getSiblingDB('fitness_tracker');


db.createCollection("users", {
  validator: {
    $jsonSchema: {
      bsonType: "object",
      required: ["id", "login", "first_name", "last_name", "password_hash"],
      properties: {
        id: { bsonType: "int" },
        login: { bsonType: "string" },
        password_hash: { bsonType: "string" }
      }
    }
  }
});

db.createCollection("exercises", {
  validator: {
    $jsonSchema: {
      bsonType: "object",
      required: ["id", "name", "category", "difficulty", "created_by"],
      properties: {
        id: { bsonType: "int" },
        name: { bsonType: "string" },
        category: { bsonType: "string", enum: ["strength", "cardio", "flexibility", "hiit"] },
        difficulty: { bsonType: "string", enum: ["beginner", "intermediate", "advanced"] }
      }
    }
  }
});

db.createCollection("workouts", {
  validator: {
    $jsonSchema: {
      bsonType: "object",
      required: ["id", "user_id", "date", "exercises"],
      properties: {
        id: { bsonType: "int" },
        user_id: { bsonType: "int" },
        date: { bsonType: "date" },
        exercises: {
          bsonType: "array",
          minItems: 1,
          items: {
            bsonType: "object",
            required: ["exercise_id", "exercise_name", "order"],
            properties: {
              exercise_id: { bsonType: "int" },
              exercise_name: { bsonType: "string" },
              order: { bsonType: "int" }
            }
          }
        }
      }
    }
  }
});

db.createCollection("counters");
db.counters.insertMany([
  { _id: "user_id", seq: 0 },
  { _id: "exercise_id", seq: 0 },
  { _id: "workout_id", seq: 0 }
]);

// Индексы
db.users.createIndex({ login: 1 }, { unique: true });
db.exercises.createIndex({ name: 1 }, { unique: true });
db.workouts.createIndex({ user_id: 1, date: -1 });

print("+");