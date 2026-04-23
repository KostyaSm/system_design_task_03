#include "workout_handlers.hpp"
#include <userver/server/handlers/base_json.hpp>
#include <userver/server/handlers/http_exceptions.hpp>
#include <userver/formats/json.hpp>
#include <userver/server/http/request.hpp>
#include "../storage/fitness_storage.hpp"
#include <string>
#include <vector>

namespace handlers {

WorkoutCreateHandler::WorkoutCreateHandler(const userver::components::ComponentConfig& config,
                                           const userver::components::ComponentContext& context)
    : userver::server::handlers::BaseJson(config, context) {
    storage_ = &context.FindComponent<storage::FitnessStorage>();
}

userver::formats::json::Value WorkoutCreateHandler::HandleRequestJson(
    const userver::server::http::HttpRequest& request,
    const userver::formats::json::Value& json,
    userver::server::request::RequestContext&) const {
    
    int user_id = 1; 
    if (auto uid = request.GetArg("user_id")) user_id = std::stoi(std::string(*uid));

    storage::Workout workout;
    workout.user_id = user_id;
    workout.title = json["title"].As<std::string>();
    workout.date_iso = json["date"].As<std::string>();

    if (json.HasMember("duration_seconds") && !json["duration_seconds"].IsNull()) {
        workout.duration_seconds = json["duration_seconds"].As<int>();
    }
    if (json.HasMember("notes") && !json["notes"].IsNull()) {
        workout.notes = json["notes"].As<std::string>();
    }

    if (json.HasMember("exercises") && !json["exercises"].IsNull()) {
        for (const auto& ex_json : json["exercises"]) {
            storage::WorkoutExercise ex;
            ex.exercise_id = ex_json["exercise_id"].As<int>();
            ex.exercise_name = ex_json["exercise_name"].As<std::string>();
            ex.order = ex_json["order"].As<int>();
            if (ex_json.HasMember("sets") && !ex_json["sets"].IsNull()) ex.sets = ex_json["sets"].As<int>();
            if (ex_json.HasMember("reps") && !ex_json["reps"].IsNull()) ex.reps = ex_json["reps"].As<int>();
            if (ex_json.HasMember("weight_kg") && !ex_json["weight_kg"].IsNull()) ex.weight_kg = ex_json["weight_kg"].As<double>();
            if (ex_json.HasMember("duration_seconds") && !ex_json["duration_seconds"].IsNull()) ex.duration_seconds = ex_json["duration_seconds"].As<int>();
            workout.exercises.push_back(ex);
        }
    }

    int workout_id = storage_->CreateWorkout(workout);

    auto result = userver::formats::json::Builder{};
    result["id"] = workout_id;
    result["title"] = workout.title;
    result["user_id"] = user_id;
    return result.ExtractValue();
}

WorkoutAddExerciseHandler::WorkoutAddExerciseHandler(const userver::components::ComponentConfig& config,
                                                     const userver::components::ComponentContext& context)
    : userver::server::handlers::BaseJson(config, context) {
    storage_ = &context.FindComponent<storage::FitnessStorage>();
}

userver::formats::json::Value WorkoutAddExerciseHandler::HandleRequestJson(
    const userver::server::http::HttpRequest& request,
    const userver::formats::json::Value& json,
    userver::server::request::RequestContext&) const {
    
    int workout_id = std::stoi(request.GetPathArg("id"));

    storage::WorkoutExercise ex;
    ex.exercise_id = json["exercise_id"].As<int>();
    ex.exercise_name = json["exercise_name"].As<std::string>();
    ex.order = json["order"].As<int>();

    if (json.HasMember("sets") && !json["sets"].IsNull()) ex.sets = json["sets"].As<int>();
    if (json.HasMember("reps") && !json["reps"].IsNull()) ex.reps = json["reps"].As<int>();
    if (json.HasMember("weight_kg") && !json["weight_kg"].IsNull()) ex.weight_kg = json["weight_kg"].As<double>();
    if (json.HasMember("duration_seconds") && !json["duration_seconds"].IsNull()) ex.duration_seconds = json["duration_seconds"].As<int>();

    if (!storage_->AddExerciseToWorkout(workout_id, ex)) {
        throw userver::server::handlers::HttpException(404, "Workout not found");
    }

    auto result = userver::formats::json::Builder{};
    result["status"] = "success";
    return result.ExtractValue();
}

UserWorkoutsHandler::UserWorkoutsHandler(const userver::components::ComponentConfig& config,
                                         const userver::components::ComponentContext& context)
    : userver::server::handlers::BaseJson(config, context) {
    storage_ = &context.FindComponent<storage::FitnessStorage>();
}

userver::formats::json::Value UserWorkoutsHandler::HandleRequestJson(
    const userver::server::http::HttpRequest& request,
    const userver::formats::json::Value&,
    userver::server::request::RequestContext&) const {
    
    int user_id = 1;
    if (auto uid = request.GetArg("user_id")) user_id = std::stoi(std::string(*uid));

    int limit = 20;
    int offset = 0;
    if (auto l = request.GetArg("limit")) limit = std::stoi(std::string(*l));
    if (auto o = request.GetArg("offset")) offset = std::stoi(std::string(*o));

    auto workouts = storage_->GetUserWorkouts(user_id, limit, offset);

    auto arr = userver::formats::json::Array{};
    for (const auto& w : workouts) {
        auto obj = userver::formats::json::Builder{};
        obj["id"] = w.id;
        obj["title"] = w.title;
        obj["date"] = w.date_iso;
        if (w.duration_seconds) obj["duration_seconds"] = *w.duration_seconds;
        if (w.notes) obj["notes"] = *w.notes;

        auto ex_arr = userver::formats::json::Array{};
        for (const auto& e : w.exercises) {
            auto e_obj = userver::formats::json::Builder{};
            e_obj["exercise_id"] = e.exercise_id;
            e_obj["exercise_name"] = e.exercise_name;
            if (e.sets) e_obj["sets"] = *e.sets;
            if (e.reps) e_obj["reps"] = *e.reps;
            if (e.weight_kg) e_obj["weight_kg"] = *e.weight_kg;
            if (e.duration_seconds) e_obj["duration_seconds"] = *e.duration_seconds;
            e_obj["order"] = e.order;
            ex_arr.PushBack(e_obj.ExtractValue());
        }
        obj["exercises"] = std::move(ex_arr);
        arr.PushBack(obj.ExtractValue());
    }

    auto result = userver::formats::json::Builder{};
    result["workouts"] = std::move(arr);
    return result.ExtractValue();
}

WorkoutsStatsHandler::WorkoutsStatsHandler(const userver::components::ComponentConfig& config,
                                           const userver::components::ComponentContext& context)
    : userver::server::handlers::BaseJson(config, context) {
    storage_ = &context.FindComponent<storage::FitnessStorage>();
}

userver::formats::json::Value WorkoutsStatsHandler::HandleRequestJson(
    const userver::server::http::HttpRequest& request,
    const userver::formats::json::Value&,
    userver::server::request::RequestContext&) const {
    
    int user_id = 1;
    if (auto uid = request.GetArg("user_id")) user_id = std::stoi(std::string(*uid));

    auto from_opt = request.GetArg("from");
    auto to_opt = request.GetArg("to");
    if (!from_opt || !to_opt) {
        throw userver::server::handlers::HttpException(400, "Missing 'from' or 'to' query parameters");
    }

    auto stats = storage_->GetWorkoutStats(user_id, std::string(*from_opt), std::string(*to_opt));

    auto result = userver::formats::json::Builder{};
    result["total_workouts"] = stats.total_workouts;
    result["total_duration_minutes"] = stats.total_duration_minutes;
    result["total_volume_kg"] = stats.total_volume_kg;
    result["exercises_completed"] = stats.exercises_completed;
    return result.ExtractValue();
}

}