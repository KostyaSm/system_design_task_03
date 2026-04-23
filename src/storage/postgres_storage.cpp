#include "postgres_storage.h"
#include <stdexcept>
#include <sstream>

namespace fitness::storage {

PostgresDB::PostgresDB(const std::string& conn_str) {
    conn_ = PQconnectdb(conn_str.c_str());
    if (PQstatus(conn_) != CONNECTION_OK) {
        std::string err = PQerrorMessage(conn_);
        PQfinish(conn_);
        throw std::runtime_error("Connection failed: " + err);
    }
}

PostgresDB::~PostgresDB() { PQfinish(conn_); }

void PostgresDB::CheckError(PGresult* res) {
    ExecStatusType status = res ? PQresultStatus(res) : PGRES_FATAL_ERROR;
    if (status != PGRES_TUPLES_OK && status != PGRES_COMMAND_OK) {
        std::string msg = res ? PQerrorMessage(conn_) : "Null result";
        if (res) PQclear(res);
        throw std::runtime_error(msg);
    }
}

std::string pg_val(PGresult* res, int row, int col) {
    return PQgetisnull(res, row, col) ? "" : PQgetvalue(res, row, col);
}

std::optional<std::string> pg_val_opt(PGresult* res, int row, int col) {
    return PQgetisnull(res, row, col) ? std::nullopt : std::make_optional(PQgetvalue(res, row, col));
}

User PostgresDB::CreateUser(const std::string& login, const std::string& fn, const std::string& ln, const std::string& email, const std::string& pass) {
    const char* params[5] = {login.c_str(), fn.c_str(), ln.c_str(), email.c_str(), pass.c_str()};
    const char* query = "INSERT INTO users (login, first_name, last_name, email, password_hash) VALUES ($1, $2, $3, $4, $5) RETURNING id, login, first_name, last_name, email, created_at;";
    PGresult* res = PQexecParams(conn_, query, 5, nullptr, params, nullptr, nullptr, 0);
    CheckError(res);
    User u;
    u.id = std::stoi(pg_val(res, 0, 0));
    u.login = pg_val(res, 0, 1);
    u.first_name = pg_val(res, 0, 2);
    u.last_name = pg_val(res, 0, 3);
    u.email = pg_val(res, 0, 4);
    u.created_at = pg_val(res, 0, 5);
    PQclear(res);
    return u;
}

std::optional<User> PostgresDB::FindUserByLogin(const std::string& login) {
    const char* params[1] = {login.c_str()};
    const char* query = "SELECT id, login, first_name, last_name, email, created_at FROM users WHERE login = $1;";
    PGresult* res = PQexecParams(conn_, query, 1, nullptr, params, nullptr, nullptr, 0);
    CheckError(res);
    if (PQntuples(res) == 0) { PQclear(res); return std::nullopt; }
    User u;
    u.id = std::stoi(pg_val(res, 0, 0));
    u.login = pg_val(res, 0, 1);
    u.first_name = pg_val(res, 0, 2);
    u.last_name = pg_val(res, 0, 3);
    u.email = pg_val(res, 0, 4);
    u.created_at = pg_val(res, 0, 5);
    PQclear(res);
    return u;
}

std::vector<User> PostgresDB::FindUsersByNameMask(const std::string& mask) {
    std::vector<User> result;
    const char* params[1] = {mask.c_str()};
    const char* query = "SELECT id, login, first_name, last_name, email, created_at FROM users WHERE first_name ILIKE $1 OR last_name ILIKE $1 ORDER BY last_name, first_name;";
    PGresult* res = PQexecParams(conn_, query, 1, nullptr, params, nullptr, nullptr, 0);
    CheckError(res);
    for (int i = 0; i < PQntuples(res); ++i) {
        User u;
        u.id = std::stoi(pg_val(res, i, 0));
        u.login = pg_val(res, i, 1);
        u.first_name = pg_val(res, i, 2);
        u.last_name = pg_val(res, i, 3);
        u.email = pg_val(res, i, 4);
        u.created_at = pg_val(res, i, 5);
        result.push_back(u);
    }
    PQclear(res);
    return result;
}

Exercise PostgresDB::CreateExercise(const std::string& name, const std::string& desc, const std::string& cat, int diff, int creator) {
    std::string d_str = std::to_string(diff);
    std::string c_str = std::to_string(creator);
    const char* params[5] = {name.c_str(), desc.c_str(), cat.c_str(), d_str.c_str(), c_str.c_str()};
    const char* query = "INSERT INTO exercises (name, description, category, difficulty, created_by) VALUES ($1, $2, $3, $4, $5) RETURNING id, name, description, category, difficulty, created_at;";
    PGresult* res = PQexecParams(conn_, query, 5, nullptr, params, nullptr, nullptr, 0);
    CheckError(res);
    Exercise e;
    e.id = std::stoi(pg_val(res, 0, 0));
    e.name = pg_val(res, 0, 1);
    e.description = pg_val(res, 0, 2);
    e.category = pg_val(res, 0, 3);
    e.difficulty = std::stoi(pg_val(res, 0, 4));
    e.created_at = pg_val(res, 0, 5);
    e.created_by = creator;
    PQclear(res);
    return e;
}

std::vector<Exercise> PostgresDB::GetExercises(int limit, int offset) {
    std::vector<Exercise> result;
    std::string l_str = std::to_string(limit);
    std::string o_str = std::to_string(offset);
    const char* params[2] = {l_str.c_str(), o_str.c_str()};
    const char* query = "SELECT id, name, description, category, difficulty, created_by, created_at FROM exercises ORDER BY name ASC LIMIT $1 OFFSET $2;";
    PGresult* res = PQexecParams(conn_, query, 2, nullptr, params, nullptr, nullptr, 0);
    CheckError(res);
    for (int i = 0; i < PQntuples(res); ++i) {
        Exercise e;
        e.id = std::stoi(pg_val(res, i, 0));
        e.name = pg_val(res, i, 1);
        e.description = pg_val(res, i, 2);
        e.category = pg_val(res, i, 3);
        e.difficulty = std::stoi(pg_val(res, i, 4));
        e.created_by = PQgetisnull(res, i, 5) ? 0 : std::stoi(pg_val(res, i, 5));
        e.created_at = pg_val(res, i, 6);
        result.push_back(e);
    }
    PQclear(res);
    return result;
}

Workout PostgresDB::CreateWorkout(int user_id, const std::string& start, const std::string& end, const std::string& notes) {
    std::string uid_str = std::to_string(user_id);
    const char* params[4] = {uid_str.c_str(), start.c_str(), end.c_str(), notes.c_str()};
    const char* query = "INSERT INTO workouts (user_id, started_at, ended_at, notes) VALUES ($1, $2, $3, $4) RETURNING id, started_at, ended_at, notes;";
    PGresult* res = PQexecParams(conn_, query, 4, nullptr, params, nullptr, nullptr, 0);
    CheckError(res);
    Workout w;
    w.id = std::stoi(pg_val(res, 0, 0));
    w.user_id = user_id;
    w.started_at = pg_val(res, 0, 1);
    w.ended_at = pg_val(res, 0, 2);
    w.notes = pg_val(res, 0, 3);
    PQclear(res);
    return w;
}

void PostgresDB::AddExerciseToWorkout(int w_id, int e_id, int sets, int reps, double weight, int duration, int order) {
    std::string w = std::to_string(w_id), e = std::to_string(e_id);
    std::string s = std::to_string(sets), r = std::to_string(reps), o = std::to_string(order);
    std::string d = std::to_string(duration);
    std::string wt_str = std::to_string(weight);
    const char* params[8] = {w.c_str(), e.c_str(), s.c_str(), r.c_str(), wt_str.c_str(), d.c_str(), o.c_str()};
    const char* query = "INSERT INTO workout_exercises (workout_id, exercise_id, sets, reps, weight_kg, duration_seconds, order_index) VALUES ($1, $2, $3, $4, $5, $6, $7);";
    PGresult* res = PQexecParams(conn_, query, 7, nullptr, params, nullptr, nullptr, 0);
    CheckError(res);
    PQclear(res);
}

std::vector<Workout> PostgresDB::GetUserWorkouts(int user_id) {
    std::vector<Workout> result;
    std::string uid = std::to_string(user_id);
    const char* params[1] = {uid.c_str()};
    const char* query = "SELECT w.id, w.user_id, w.started_at, w.ended_at, w.notes FROM workouts w WHERE w.user_id = $1 ORDER BY w.started_at DESC;";
    PGresult* res = PQexecParams(conn_, query, 1, nullptr, params, nullptr, nullptr, 0);
    CheckError(res);
    for (int i = 0; i < PQntuples(res); ++i) {
        Workout w;
        w.id = std::stoi(pg_val(res, i, 0));
        w.user_id = std::stoi(pg_val(res, i, 1));
        w.started_at = pg_val(res, i, 2);
        w.ended_at = pg_val(res, i, 3);
        w.notes = pg_val(res, i, 4);
        result.push_back(w);
    }
    PQclear(res);
    return result;
}

Stats PostgresDB::GetWorkoutStats(int user_id, const std::string& from, const std::string& to) {
    std::string uid = std::to_string(user_id);
    const char* params[3] = {uid.c_str(), from.c_str(), to.c_str()};
    const char* query = "SELECT COUNT(DISTINCT w.id), COUNT(we.id), COALESCE(SUM(COALESCE(we.duration_seconds, 0)), 0), COALESCE(AVG(EXTRACT(EPOCH FROM (w.ended_at - w.started_at))), 0) FROM workouts w LEFT JOIN workout_exercises we ON w.id = we.workout_id WHERE w.user_id = $1 AND w.started_at BETWEEN $2 AND $3;";
    PGresult* res = PQexecParams(conn_, query, 3, nullptr, params, nullptr, nullptr, 0);
    CheckError(res);
    Stats s;
    s.total_workouts = PQgetisnull(res, 0, 0) ? 0 : std::stoi(pg_val(res, 0, 0));
    s.total_exercises = PQgetisnull(res, 0, 1) ? 0 : std::stoi(pg_val(res, 0, 1));
    s.total_duration = PQgetisnull(res, 0, 2) ? 0 : std::stoll(pg_val(res, 0, 2));
    s.avg_duration = PQgetisnull(res, 0, 3) ? 0.0 : std::stod(pg_val(res, 0, 3));
    PQclear(res);
    return s;
}

}