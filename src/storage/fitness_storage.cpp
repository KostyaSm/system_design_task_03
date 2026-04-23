#include "fitness_storage.hpp"
#include <userver/storages/mongodb/client.hpp>
#include <userver/storages/bson.hpp>
#include <userver/utils/datetime.hpp>
#include <fmt/format.h>





namespace storage {

using namespace userver::storages::mongodb;
using namespace userver::storages::bson;

FitnessStorage::FitnessStorage(ClientPtr client) : client_(std::move(client)) {}

int FitnessStorage::GetNextId(const std::string& counter_name) {
    auto res = client_->GetCollection("counters").FindOneAndUpdate(
        MakeObj("_id", counter_name),
        MakeObj("$inc", MakeObj("seq", 1)),
        OptionsFindOneAndUpdate().Upsert(true).ReturnDocument(ReturnDocument::kAfter)
    ).GetDocument();
    return res["seq"].As<int>();
}

int FitnessStorage::RegisterUser(const std::string& login, const std::string& first_name,
                                 const std::string& last_name, const std::string& password_hash,
                                 const std::optional<std::string>& email) {
    int id = GetNextId("user_id");
    client_->GetCollection("users").Insert(MakeObj(
        "id", id, "login", login, "first_name", first_name, "last_name", last_name,
        "password_hash", password_hash, "email", email.value_or(""),
        "created_at", std::chrono::system_clock::now()
    ));
    return id;
}


std::optional<User> FitnessStorage::FindUserByLogin(const std::string& login) const {
    auto doc = client_->GetCollection("users").FindOne(MakeObj("login", login));
    if (!doc) return std::nullopt;
    auto& d = doc.GetDocument();
    return User{
        d["id"].As<int>(), d["login"].AsString(), d["first_name"].AsString(),
        d["last_name"].AsString(),
        d["email"].IsNull() ? std::nullopt : std::optional(d["email"].AsString()),
        d["password_hash"].AsString()
    };
}

std::vector<User> FitnessStorage::SearchUsersByMask(const std::string& mask) const {
    std::vector<User> result;
    auto cursor = client_->GetCollection("users").Find(
        MakeObj("$or", MakeArray(
            MakeObj("first_name", MakeObj("$regex", mask, "$options", "i")),
            MakeObj("last_name", MakeObj("$regex", mask, "$options", "i"))
        ))
    );
    while (cursor.Next()) {
        auto& d = cursor.GetDocument();
        result.push_back(User{
            d["id"].As<int>(), d["login"].AsString(), d["first_name"].AsString(),
            d["last_name"].AsString(),
            d["email"].IsNull() ? std::nullopt : std::optional(d["email"].AsString()),
            d["password_hash"].AsString()
        });
    }
    return result;
}

#додел
int FitnessStorage::CreateExercise(const Exercise& ex) {
    int id = GetNextId("exercise_id");
    client_->GetCollection("exercises").Insert(MakeObj(
        "id", id, "name", ex.name, "description", ex.description.value_or(""),
        "category", ex.category, "muscle_groups", MakeArray(ex.muscle_groups.begin(), ex.muscle_groups.end()),
        "difficulty", ex.difficulty, "created_by", ex.created_by, "is_public", ex.is_public,
        "created_at", std::chrono::system_clock::now()
    ));
    return id;
}

std::vector<Exercise> FitnessStorage::GetExercises(const std::optional<std::string>& category,
                                                   const std::optional<std::string>& difficulty) const {
    std::vector<Exercise> result;
    auto filter = MakeObj("is_public", true);
    if (category) filter["category"] = *category;
    if (difficulty) filter["difficulty"] = *difficulty;

    auto cursor = client_->GetCollection("exercises").Find(filter);
    while (cursor.Next()) {
        auto& d = cursor.GetDocument();
        std::vector<std::string> mg;
        for (const auto& v : d["muscle_groups"].AsArray()) mg.push_back(v.AsString());
        result.push_back(Exercise{
            d["id"].As<int>(), d["name"].AsString(),
            d["description"].IsNull() ? std::nullopt : std::optional(d["description"].AsString()),
            d["category"].AsString(), mg, d["difficulty"].AsString(),
            d["created_by"].As<int>(), d["is_public"].As<bool>()
        });
    }
    return result;
}

int FitnessStorage::CreateWorkout(const Workout& w) {
    int id = GetNextId("workout_id");
    auto exercises_arr = MakeArray();
    for (const auto& ex : w.exercises) {
        exercises_arr.Append(MakeObj(
            "exercise_id", ex.exercise_id, "exercise_name", ex.exercise_name,
            "sets", ex.sets.value_or(nullptr), "reps", ex.reps.value_or(nullptr),
            "weight_kg", ex.weight_kg.value_or(nullptr), "duration_seconds", ex.duration_seconds.value_or(nullptr),
            "order", ex.order
        ));
    }
    client_->GetCollection("workouts").Insert(MakeObj(
        "id", id, "user_id", w.user_id, "title", w.title, "date", w.date_iso,
        "duration_seconds", w.duration_seconds.value_or(nullptr), "notes", w.notes.value_or(""),
        "exercises", exercises_arr, "created_at", std::chrono::system_clock::now()
    ));
    return id;
}

bool FitnessStorage::AddExerciseToWorkout(int workout_id, const WorkoutExercise& ex) {
    auto res = client_->GetCollection("workouts").UpdateOne(
        MakeObj("id", workout_id),
        MakeObj("$push", MakeObj("exercises", MakeObj(
            "exercise_id", ex.exercise_id, "exercise_name", ex.exercise_name,
            "sets", ex.sets.value_or(nullptr), "reps", ex.reps.value_or(nullptr),
            "weight_kg", ex.weight_kg.value_or(nullptr), "duration_seconds", ex.duration_seconds.value_or(nullptr),
            "order", ex.order
        )))
    );
    return res.GetModifiedCount() > 0;
}

std::vector<Workout> FitnessStorage::GetUserWorkouts(int user_id, int limit, int offset) const {
    std::vector<Workout> result;
    auto cursor = client_->GetCollection("workouts").Find(MakeObj("user_id", user_id))
        .Sort(MakeObj("date", -1)).Skip(offset).Limit(limit);
    while (cursor.Next()) {
        auto& d = cursor.GetDocument();
        Workout w;
        w.id = d["id"].As<int>();
        w.user_id = d["user_id"].As<int>();
        w.title = d["title"].AsString();
        w.date_iso = d["date"].As<std::string>();
        w.duration_seconds = d["duration_seconds"].IsNull() ? std::nullopt : std::optional(d["duration_seconds"].As<int>());
        w.notes = d["notes"].IsNull() ? std::nullopt : std::optional(d["notes"].AsString());

        for (const auto& e : d["exercises"].AsArray()) {
            w.exercises.push_back(WorkoutExercise{
                e["exercise_id"].As<int>(), e["exercise_name"].AsString(),
                e["sets"].IsNull() ? std::nullopt : std::optional(e["sets"].As<int>()),
                e["reps"].IsNull() ? std::nullopt : std::optional(e["reps"].As<int>()),
                e["weight_kg"].IsNull() ? std::nullopt : std::optional(e["weight_kg"].As<double>()),
                e["duration_seconds"].IsNull() ? std::nullopt : std::optional(e["duration_seconds"].As<int>()),
                e["order"].As<int>()
            });
        }
        result.push_back(w);
    }
    return result;
}


#додел
Statistics FitnessStorage::GetWorkoutStats(int user_id, const std::string& date_from, const std::string& date_to) const {
    auto pipeline = MakeArray(
        MakeObj("$match", MakeObj("user_id", user_id, "date", MakeObj("$gte", date_from, "$lte", date_to))),
        MakeObj("$unwind", "$exercises"),
        MakeObj("$group", MakeObj(
            "_id", "$user_id",
            "total_workouts", MakeObj("$sum", 1),
            "total_exercises", MakeObj("$sum", 1),
            "total_volume", MakeObj("$sum", MakeObj("$ifNull", MakeArray("$exercises.weight_kg", 0)))
        ))
    );
    auto cursor = client_->GetCollection("workouts").Aggregate(pipeline);
    Statistics stats{0, 0, 0.0, 0};
    if (cursor.Next()) {
        auto& d = cursor.GetDocument();
        stats.total_workouts = d["total_workouts"].As<int>();
        stats.exercises_completed = d["total_exercises"].As<int>();
        stats.total_volume_kg = d["total_volume"].As<double>();
    }
    return stats;
}

}
