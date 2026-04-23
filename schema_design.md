# Описание проекта
Приложение для учёта тренировок, упражнений и прогресса пользователей.  
Аналог MyFitnessPal

# Сущности системы


User - Пользователь приложения        login, name, email, settings
Exercise - Шаблон упражнения в библиотеке        name, category, muscle_groups, difficulty
Workout - Конкретная тренировка пользователя        date, duration, список выполненных упражнений

---

# Структура коллекций MongoDB

# Коллекция `users`

```javascript
{
  _id: ObjectId,
  id: int,
  login: string,
  first_name: string,
  last_name: string,
  email: string | null,
  password_hash: string,
  created_at: date,
  updated_at: date,
  settings: {
    unit_system: "metric" | "imperial",
    privacy: "public" | "private"
  },
  tokens: [
    {
      token: string,
      created_at: date,
      expires_at: date
    }
  ]
}
```

**Индексы:**
```javascript
db.users.createIndex({ login: 1 }, { unique: true, collation: { locale: "ru", strength: 2 } })
db.users.createIndex({ email: 1 }, { unique: true, sparse: true })
db.users.createIndex({ first_name_lc: 1, last_name_lc: 1 })  #по маске
```

# Коллекция `exercises`

```javascript
{
  _id: ObjectId,
  id: int,
  name: string,
  description: string | null, 
  category: "strength" | "cardio" | "flexibility" | "hiit",
  muscle_groups: [string],
  difficulty: "beginner" | "intermediate" | "advanced",
  created_by: int,
  created_at: date,
  is_public: boolean 
}
```

**Индексы:**
```javascript
db.exercises.createIndex({ name: 1 }, { unique: true, collation: { locale: "ru", strength: 2 } })
db.exercises.createIndex({ category: 1, difficulty: 1 })
db.exercises.createIndex({ created_by: 1 })
db.exercises.createIndex({ muscle_groups: 1 })
```

---

### Коллекция `workouts`

```javascript
{
  _id: ObjectId,
  id: int,
  user_id: int,
  title: string,
  date: date,
  duration_seconds: int | null,
  notes: string | null,
  exercises: [
    {
      exercise_id: int,
      exercise_name: string,
      sets: int | null,
      reps: int | null,
      weight_kg: double | null,
      duration_seconds: int | null,
      rest_seconds: int | null,
      notes: string | null,
      order: int
    }
  ],
  created_at: date,
  updated_at: date
}
```

**Индексы:**
```javascript
db.workouts.createIndex({ user_id: 1, date: -1 })
db.workouts.createIndex({ user_id: 1, "exercises.exercise_id": 1 })
db.workouts.createIndex({ date: 1 })
```

---

### Коллекция `counters` (техническая)

```javascript
{
  _id: "user_id" | "exercise_id" | "workout_id",
  seq: int
}
```
