#pragma once
#include <string>
#include <vector>
#include <optional>
#include <libpq-fe.h>

namespace fitness::storage {

struct User { int id; std::string login, first_name, last_name, email, created_at; };
struct Exercise { int id, created_by, difficulty; std::string name, description, category, created_at; };
struct Workout { int id, user_id; std::string started_at, ended_at, notes; };
struct WorkoutExercise { int id, workout_id, exercise_id, order_index; std::optional<int> sets, reps, duration_seconds; std::optional<double> weight_kg; };
struct Stats { int total_workouts, total_exercises; long long total_duration; double avg_duration; };

class PostgresDB {
public:
    explicit PostgresDB(const std::string& conn_str);
    ~PostgresDB();

    User CreateUser(const std::string& login, const std::string& fn, const std::string& ln, const std::string& email, const std::string& pass);
    std::optional<User> FindUserByLogin(const std::string& login);
    std::vector<User> FindUsersByNameMask(const std::string& mask);

    Exercise CreateExercise(const std::string& name, const std::string& desc, const std::string& cat, int diff, int creator);
    std::vector<Exercise> GetExercises(int limit, int offset);

    Workout CreateWorkout(int user_id, const std::string& start, const std::string& end, const std::string& notes);
    void AddExerciseToWorkout(int w_id, int e_id, int sets, int reps, double weight, int duration, int order);

    std::vector<Workout> GetUserWorkouts(int user_id);
    Stats GetWorkoutStats(int user_id, const std::string& from, const std::string& to);

private:
    PGconn* conn_;
    void CheckError(PGresult* res);
    PGresult* ExecParams(const std::string& query, const std::vector<const char*>& params, const std::vector<int>& lengths, const std::vector<int>& formats);
};

}