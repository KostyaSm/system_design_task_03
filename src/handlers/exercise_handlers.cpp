#include "exercise_handlers.hpp"
#include <userver/server/handlers/base_json.hpp>
#include <userver/server/handlers/http_exceptions.hpp>
#include <userver/formats/json.hpp>
#include <userver/server/http/request.hpp>
#include "../storage/fitness_storage.hpp"
#include <optional>
#include <vector>

namespace handlers {

ExerciseCreateHandler::ExerciseCreateHandler(const userver::components::ComponentConfig& config,
                                             const userver::components::ComponentContext& context)
    : userver::server::handlers::BaseJson(config, context) {
    storage_ = &context.FindComponent<storage::FitnessStorage>();
}

userver::formats::json::Value ExerciseCreateHandler::HandleRequestJson(
    const userver::server::http::HttpRequest&,
    const userver::formats::json::Value& json,
    userver::server::request::RequestContext&) const {
    
    std::string name = json["name"].As<std::string>();
    std::string category = json["category"].As<std::string>();
    std::string difficulty = json["difficulty"].As<std::string>();

    storage::Exercise ex;
    ex.name = name;
    ex.category = category;
    ex.difficulty = difficulty;
    ex.description = json.HasMember("description") && !json["description"].IsNull() 
                     ? json["description"].As<std::string>() 
                     : std::nullopt;
    ex.created_by = 1; 
    ex.is_public = true;

    std::vector<std::string> muscles;
    if (json.HasMember("muscle_groups") && !json["muscle_groups"].IsNull()) {
        for (const auto& m : json["muscle_groups"]) {
            muscles.push_back(m.As<std::string>());
        }
    }
    ex.muscle_groups = muscles;

    int id = storage_->CreateExercise(ex);

    auto result = userver::formats::json::Builder{};
    result["id"] = id;
    result["name"] = name;
    result["category"] = category;
    result["difficulty"] = difficulty;
    return result.ExtractValue();
}

ExerciseListHandler::ExerciseListHandler(const userver::components::ComponentConfig& config,
                                         const userver::components::ComponentContext& context)
    : userver::server::handlers::BaseJson(config, context) {
    storage_ = &context.FindComponent<storage::FitnessStorage>();
}

userver::formats::json::Value ExerciseListHandler::HandleRequestJson(
    const userver::server::http::HttpRequest& request,
    const userver::formats::json::Value&,
    userver::server::request::RequestContext&) const {
    
    std::optional<std::string> category;
    std::optional<std::string> difficulty;

    if (auto cat = request.GetArg("category")) category = std::string(*cat);
    if (auto diff = request.GetArg("difficulty")) difficulty = std::string(*diff);

    auto exercises = storage_->GetExercises(category, difficulty);
    
    auto arr = userver::formats::json::Array{};
    for (const auto& ex : exercises) {
        auto obj = userver::formats::json::Builder{};
        obj["id"] = ex.id;
        obj["name"] = ex.name;
        obj["category"] = ex.category;
        obj["difficulty"] = ex.difficulty;
        obj["is_public"] = ex.is_public;
        if (ex.description) obj["description"] = *ex.description;
        
        auto mg_arr = userver::formats::json::Array{};
        for (const auto& mg : ex.muscle_groups) mg_arr.PushBack(mg);
        obj["muscle_groups"] = std::move(mg_arr);
        
        arr.PushBack(obj.ExtractValue());
    }

    auto result = userver::formats::json::Builder{};
    result["exercises"] = std::move(arr);
    return result.ExtractValue();
}

}