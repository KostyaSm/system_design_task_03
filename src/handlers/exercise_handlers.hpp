#pragma once

#include <userver/server/handlers/base_json.hpp>
#include <userver/components/component_context.hpp>
#include <userver/components/component_config.hpp>
#include "../storage/fitness_storage.hpp"

namespace handlers {

class ExerciseCreateHandler final : public userver::server::handlers::BaseJson {
public:
    static constexpr std::string_view kName = "handler-exercises-create";
    ExerciseCreateHandler(const userver::components::ComponentConfig& config, const userver::components::ComponentContext& context);
    userver::formats::json::Value HandleRequestJson(const userver::server::http::HttpRequest&, const userver::formats::json::Value&, userver::server::request::RequestContext&) const override;
private:
    storage::FitnessStorage* storage_;
};

class ExerciseListHandler final : public userver::server::handlers::BaseJson {
public:
    static constexpr std::string_view kName = "handler-exercises-list";
    ExerciseListHandler(const userver::components::ComponentConfig& config, const userver::components::ComponentContext& context);
    userver::formats::json::Value HandleRequestJson(const userver::server::http::HttpRequest&, const userver::formats::json::Value&, userver::server::request::RequestContext&) const override;
private:
    storage::FitnessStorage* storage_;
};

}