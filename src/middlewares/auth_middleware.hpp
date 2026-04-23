#pragma once

#include <userver/server/handlers/middleware_base.hpp>
#include <userver/components/component_context.hpp>
#include <userver/components/component_config.hpp>

namespace middlewares {

class AuthMiddleware final : public userver::server::handlers::MiddlewareBase {
public:
    static constexpr std::string_view kName = "auth-middleware";
    AuthMiddleware(const userver::components::ComponentConfig& config, const userver::components::ComponentContext& context);
    void Handle(userver::server::http::HttpRequest& request, userver::server::request::RequestContext& ctx) const override;
};

}