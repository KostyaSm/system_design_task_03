INSERT INTO users (login, first_name, last_name, email, password_hash)
VALUES ($1, $2, $3, $4, $5)
RETURNING id, login, first_name, last_name, email, created_at;

SELECT id, login, first_name, last_name, email, created_at
FROM users
WHERE login = $1;

SELECT id, login, first_name, last_name, email, created_at
FROM users
WHERE CONCAT(first_name, ' ', last_name) ILIKE $1;

INSERT INTO exercises (name, description, category, difficulty, created_by)
VALUES ($1, $2, $3, $4, $5)
RETURNING id, name, description, category, difficulty, created_at;

SELECT id, name, description, category, difficulty, created_by, created_at
FROM exercises
ORDER BY name ASC
LIMIT $1 OFFSET $2;

INSERT INTO workouts (user_id, started_at, ended_at, notes)
VALUES ($1, $2, $3, $4)
RETURNING id, started_at, ended_at, notes;

INSERT INTO workout_exercises (workout_id, exercise_id, sets, reps, weight_kg, duration_seconds, order_index)
VALUES ($1, $2, $3, $4, $5, $6, $7)
RETURNING id;

SELECT
    w.id AS workout_id,
    w.started_at,
    w.ended_at,
    w.notes,
    we.order_index,
    e.name AS exercise_name,
    e.category,
    we.sets,
    we.reps,
    we.weight_kg,
    we.duration_seconds
FROM workouts w
JOIN workout_exercises we ON w.id = we.workout_id
JOIN exercises e ON we.exercise_id = e.id
WHERE w.user_id = $1
ORDER BY w.started_at DESC, we.order_index ASC;

SELECT
    COUNT(DISTINCT w.id) AS total_workouts,
    COUNT(we.id) AS total_exercises_logged,
    COALESCE(SUM(COALESCE(we.duration_seconds, 0)), 0) AS total_duration_seconds,
    COALESCE(AVG(EXTRACT(EPOCH FROM (w.ended_at - w.started_at))), 0) AS avg_workout_duration_seconds
FROM workouts w
LEFT JOIN workout_exercises we ON w.id = we.workout_id
WHERE w.user_id = $1
  AND w.started_at BETWEEN $2 AND $3;