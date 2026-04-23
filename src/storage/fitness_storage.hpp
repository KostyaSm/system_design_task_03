#pragma once
#include <userver/storages/mongodb/client.hpp>
#include <string>
#include <vector>
#include <optional>


namespace storage {

struct User {
    int id;
    std::string login;
    std::string first_name;
    std::string last_name;
    std::optional<std::string> email;
    std::string password_hash;
};
struct Exercise {
    int id;
    std::string name;
    std::optional<std::string> description;
    std::string category;
    std::vector<std::string> muscle_groups;
    std::string difficulty;
    int created_by;
    bool is_public;
};

struct WorkoutExercise {
    int exercise_id;
    std::string exercise_name;
    std::optional<int> sets;
    std::optional<int> reps;
    std::optional<double> weight_kg;
    std::optional<int> duration_seconds;
    int order;
};

struct Workout {
    int id;
    int user_id;
    std::string title;
    std::string date_iso;
    std::optional<int> duration_seconds;
    std::optional<std::string> notes;
    std::vector<WorkoutExercise> exercises;
};

struct Statistics {
    int total_workouts;
    int total_duration_minutes;
    double total_volume_kg;
    int exercises_completed;
};

class FitnessStorage {
public:
    explicit FitnessStorage(userver::storages::mongodb::ClientPtr client);

    int RegisterUser(const std::string& login, const std::string& first_name,
                     const std::string& last_name, const std::string& password_hash,
                     const std::optional<std::string>& email);
    std::optional<User> FindUserByLogin(const std::string& login) const;
    std::vector<User> SearchUsersByMask(const std::string& mask) const;

    int CreateExercise(const Exercise& ex);
    std::vector<Exercise> GetExercises(const std::optional<std::string>& category,
                                       const std::optional<std::string>& difficulty) const;

    int CreateWorkout(const Workout& workout);
    bool AddExerciseToWorkout(int workout_id, const WorkoutExercise& exercise);
    std::vector<Workout> GetUserWorkouts(int user_id, int limit, int offset) const;
    Statistics GetWorkoutStats(int user_id, const std::string& date_from, const std::string& date_to) const;

private:
    userver::storages::mongodb::ClientPtr client_;
    int GetNextId(const std::string& counter_name);
};

}