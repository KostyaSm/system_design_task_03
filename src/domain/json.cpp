#include "json.hpp"
#include <userver/formats/json/value_builder.hpp>

namespace domain::json {

using namespace userver::formats::json;
 
storage::User ParseUser(const Value& json) {
    storage::User u;
    u.id = json.HasMember("id") ? json["id"].As<int>() : 0;
    u.login = json["login"].As<std::string>();
    u.first_name = json["first_name"].As<std::string>();
    u.last_name = json["last_name"].As<std::string>();
    if (json.HasMember("password_hash") && !json["password_hash"].IsNull()) {
        u.password_hash = json["password_hash"].As<std::string>();
    }
    if (json.HasMember("email") && !json["email"].IsNull()) {
        u.email = json["email"].As<std::string>();
    }
    return u;
}


Value SerializeUser(const storage::User& user) {
    ValueBuilder vb{Object};
    vb["id"] = user.id;
    vb["login"] = user.login;
    vb["first_name"] = user.first_name;
    vb["last_name"] = user.last_name;
    vb["email"] = user.email.value_or("");
    return vb.ExtractValue();
}
    
storage::Exercise ParseExercise(const Value& json) {
    storage::Exercise ex;
    ex.id = json.HasMember("id") ? json["id"].As<int>() : 0;
    ex.name = json["name"].As<std::string>();
    ex.category = json["category"].As<std::string>();
    ex.difficulty = json["difficulty"].As<std::string>();
    ex.created_by = json.HasMember("created_by") ? json["created_by"].As<int>() : 0;
    ex.is_public = json.HasMember("is_public") ? json["is_public"].As<bool>() : true;
    if (json.HasMember("description") && !json["description"].IsNull()) {
        ex.description = json["description"].As<std::string>();
    }
    if (json.HasMember("muscle_groups") && !json["muscle_groups"].IsNull()) {
        std::vector<std::string> mg;
        for (const auto& m : json["muscle_groups"]) mg.push_back(m.As<std::string>());
        ex.muscle_groups = std::move(mg);
    }
    return ex;
}

Value SerializeExercise(const storage::Exercise& ex) {
    ValueBuilder vb{Object};
    vb["id"] = ex.id;
    vb["name"] = ex.name;
    vb["category"] = ex.category;
    vb["difficulty"] = ex.difficulty;
    vb["is_public"] = ex.is_public;
    if (ex.description) vb["description"] = *ex.description;
    ValueBuilder mg_builder{Array};
    for (const auto& m : ex.muscle_groups) mg_builder.PushBack(m);
    vb["muscle_groups"] = mg_builder.ExtractValue();
    return vb.ExtractValue();
}



storage::WorkoutExercise ParseWorkoutExercise(const Value& json) {
    storage::WorkoutExercise ex;
    ex.exercise_id = json["exercise_id"].As<int>();
    ex.exercise_name = json["exercise_name"].As<std::string>();
    ex.order = json.HasMember("order") ? json["order"].As<int>() : 0;
    if (json.HasMember("sets") && !json["sets"].IsNull()) ex.sets = json["sets"].As<int>();
    if (json.HasMember("reps") && !json["reps"].IsNull()) ex.reps = json["reps"].As<int>();
    if (json.HasMember("weight_kg") && !json["weight_kg"].IsNull()) ex.weight_kg = json["weight_kg"].As<double>();
    if (json.HasMember("duration_seconds") && !json["duration_seconds"].IsNull()) ex.duration_seconds = json["duration_seconds"].As<int>();
    return ex;
}

Value SerializeWorkoutExercise(const storage::WorkoutExercise& ex) {
    ValueBuilder vb{Object};
    vb["exercise_id"] = ex.exercise_id;
    vb["exercise_name"] = ex.exercise_name;
    vb["order"] = ex.order;
    if (ex.sets) vb["sets"] = *ex.sets;
    if (ex.reps) vb["reps"] = *ex.reps;
    if (ex.weight_kg) vb["weight_kg"] = *ex.weight_kg;
    if (ex.duration_seconds) vb["duration_seconds"] = *ex.duration_seconds;
    return vb.ExtractValue();
}

storage::Workout ParseWorkout(const Value& json) {
    storage::Workout w;
    w.id = json.HasMember("id") ? json["id"].As<int>() : 0;
    w.user_id = json.HasMember("user_id") ? json["user_id"].As<int>() : 0;
    w.title = json["title"].As<std::string>();
    w.date_iso = json["date"].As<std::string>();
    if (json.HasMember("duration_seconds") && !json["duration_seconds"].IsNull()) w.duration_seconds = json["duration_seconds"].As<int>();
    if (json.HasMember("notes") && !json["notes"].IsNull()) w.notes = json["notes"].As<std::string>();
    if (json.HasMember("exercises") && !json["exercises"].IsNull()) {
        std::vector<storage::WorkoutExercise> exercises;
        for (const auto& e : json["exercises"]) {
            exercises.push_back(ParseWorkoutExercise(e));
        }
        w.exercises = std::move(exercises);
    }
    return w;
}

Value SerializeWorkout(const storage::Workout& w) {
    ValueBuilder vb{Object};
    vb["id"] = w.id;
    vb["user_id"] = w.user_id;
    vb["title"] = w.title;
    vb["date"] = w.date_iso;
    if (w.duration_seconds) vb["duration_seconds"] = *w.duration_seconds;
    if (w.notes) vb["notes"] = *w.notes;
    ValueBuilder ex_builder{Array};
    for (const auto& e : w.exercises) ex_builder.PushBack(SerializeWorkoutExercise(e));
    vb["exercises"] = ex_builder.ExtractValue();
    return vb.ExtractValue();
}

Value SerializeStats(const storage::Statistics& stats) {
    ValueBuilder vb{Object};
    vb["total_workouts"] = stats.total_workouts;
    vb["total_duration_minutes"] = stats.total_duration_minutes;
    vb["total_volume_kg"] = stats.total_volume_kg;
    vb["exercises_completed"] = stats.exercises_completed;
    return vb.ExtractValue();
}

}
