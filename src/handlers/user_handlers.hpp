#pragma once

#include <userver/server/handlers/base_json.hpp>
#include <userver/components/component_context.hpp>
#include <userver/components/component_config.hpp>
#include "../storage/fitness_storage.hpp"

namespace handlers {

class UserByLoginHandler final : public userver::server::handlers::BaseJson {
public:
    static constexpr std::string_view kName = "handler-user-by-login";
    UserByLoginHandler(const userver::components::ComponentConfig& config, const userver::components::ComponentContext& context);
    userver::formats::json::Value HandleRequestJson(const userver::server::http::HttpRequest&, const userver::formats::json::Value&, userver::server::request::RequestContext&) const override;
private:
    storage::FitnessStorage* storage_;
};

class UserSearchHandler final : public userver::server::handlers::BaseJson {
public:
    static constexpr std::string_view kName = "handler-user-search";
    UserSearchHandler(const userver::components::ComponentConfig& config, const userver::components::ComponentContext& context);
    userver::formats::json::Value HandleRequestJson(const userver::server::http::HttpRequest&, const userver::formats::json::Value&, userver::server::request::RequestContext&) const override;
private:
    storage::FitnessStorage* storage_;
};

} 