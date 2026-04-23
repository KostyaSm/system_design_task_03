#include "auth_handlers.hpp"
#include <userver/server/handlers/base_json.hpp>
#include <userver/server/handlers/http_exceptions.hpp>
#include <userver/formats/json.hpp>
#include <fmt/format.h>
#include "../storage/fitness_storage.hpp"
#include "../utils/http_utils.hpp"

namespace handlers {
AuthRegisterHandler::AuthRegisterHandler(const userver::components::ComponentConfig& config,
                                         const userver::components::ComponentContext& context)
    : userver::server::handlers::BaseJson(config, context) {
    storage_ = &context.FindComponent<storage::FitnessStorage>();
}

userver::formats::json::Value AuthRegisterHandler::HandleRequestJson(
    const userver::server::http::HttpRequest&,
    const userver::formats::json::Value& json,
    userver::server::request::RequestContext&) const {
    
    std::string login = json["login"].As<std::string>();
    std::string first_name = json["first_name"].As<std::string>();
    std::string last_name = json["last_name"].As<std::string>();
    std::string password = json["password"].As<std::string>();

    if (storage_->FindUserByLogin(login)) {
        throw userver::server::handlers::HttpException(409, "Login already exists");
    }

    std::string password_hash = "pw$" + password;
    std::optional<std::string> email;
    if (json.HasMember("email") && !json["email"].IsNull()) {
        email = json["email"].As<std::string>();
    }

    int user_id = storage_->RegisterUser(login, first_name, last_name, password_hash, email);

    auto result = userver::formats::json::Builder{};
    result["id"] = user_id;
    result["login"] = login;
    result["first_name"] = first_name;
    result["last_name"] = last_name;
    result["email"] = email.value_or("");
    return result.ExtractValue();
}

AuthLoginHandler::AuthLoginHandler(const userver::components::ComponentConfig& config,
                                   const userver::components::ComponentContext& context)
    : userver::server::handlers::BaseJson(config, context) {
    storage_ = &context.FindComponent<storage::FitnessStorage>();
}

userver::formats::json::Value AuthLoginHandler::HandleRequestJson(
    const userver::server::http::HttpRequest&,
    const userver::formats::json::Value& json,
    userver::server::request::RequestContext&) const {
    
    std::string login = json["login"].As<std::string>();
    std::string password = json["password"].As<std::string>();

    auto user = storage_->FindUserByLogin(login);
    if (!user || user->password_hash != "pw$" + password) {
        throw userver::server::handlers::HttpException(401, "Invalid credentials");
    }

    std::string token = "token_" + std::to_string(user->id) + "_" + std::to_string(std::time(nullptr));

    auto result = userver::formats::json::Builder{};
    result["user_id"] = user->id;
    result["token"] = token;
    return result.ExtractValue();
}




}