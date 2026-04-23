#include "user_handlers.hpp"
#include <userver/server/handlers/base_json.hpp>
#include <userver/server/handlers/http_exceptions.hpp>
#include <userver/formats/json.hpp>
#include <userver/server/http/request.hpp>
#include "../storage/fitness_storage.hpp"

namespace handlers {

UserByLoginHandler::UserByLoginHandler(const userver::components::ComponentConfig& config,
                                       const userver::components::ComponentContext& context)
    : userver::server::handlers::BaseJson(config, context) {
    storage_ = &context.FindComponent<storage::FitnessStorage>();
}


userver::formats::json::Value UserByLoginHandler::HandleRequestJson(
    const userver::server::http::HttpRequest& request,
    const userver::formats::json::Value&,
    userver::server::request::RequestContext&) const {
    
    auto login_opt = request.GetArg("login");
    if (!login_opt) {
        throw userver::server::handlers::HttpException(400, "Missing 'login' query parameter");
    }

    std::string login = std::string(*login_opt);
    auto user = storage_->FindUserByLogin(login);
    if (!user) {
        throw userver::server::handlers::HttpException(404, "User not found");
    }

    auto result = userver::formats::json::Builder{};
    result["id"] = user->id;
    result["login"] = user->login;
    result["first_name"] = user->first_name;
    result["last_name"] = user->last_name;
    result["email"] = user->email.value_or("");
    return result.ExtractValue();
}

UserSearchHandler::UserSearchHandler(const userver::components::ComponentConfig& config,
                                     const userver::components::ComponentContext& context)
    : userver::server::handlers::BaseJson(config, context) {
    storage_ = &context.FindComponent<storage::FitnessStorage>();
}

userver::formats::json::Value UserSearchHandler::HandleRequestJson(
    const userver::server::http::HttpRequest& request,
    const userver::formats::json::Value&,
    userver::server::request::RequestContext&) const {
    
    auto mask_opt = request.GetArg("mask");
    if (!mask_opt) {
        throw userver::server::handlers::HttpException(400, "Missing 'mask' query parameter");
    }


    std::string mask = std::string(*mask_opt);
    auto users = storage_->SearchUsersByMask(mask);
    
    auto arr = userver::formats::json::Array{};
    for (const auto& u : users) {
        auto obj = userver::formats::json::Builder{};
        obj["id"] = u.id;
        obj["login"] = u.login;
        obj["first_name"] = u.first_name;
        obj["last_name"] = u.last_name;
        obj["email"] = u.email.value_or("");
        arr.PushBack(obj.ExtractValue());
    }

    auto result = userver::formats::json::Builder{};
    result["users"] = std::move(arr);
    return result.ExtractValue();
}

}
