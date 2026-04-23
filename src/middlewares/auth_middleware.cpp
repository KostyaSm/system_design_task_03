#include "auth_middleware.hpp"
#include <userver/server/http/http_status.hpp>
#include <userver/server/request/request_context.hpp>

namespace middlewares {

AuthMiddleware::AuthMiddleware(const userver::components::ComponentConfig& config,
                               const userver::components::ComponentContext& context)
    : userver::server::handlers::MiddlewareBase(config, context) {}

void AuthMiddleware::Handle(userver::server::http::HttpRequest& request, userver::server::request::RequestContext& ctx) const {
    if (request.GetMethod() == userver::server::http::HttpMethod::kOptions) return;

    std::string auth = request.GetHeader("Authorization");
    if (auth.size() < 7 || auth.substr(0, 7) != "Bearer ") {
        request.SetResponseStatus(userver::server::http::HttpStatus::kUnauthorized);
        request.SendString("Unauthorized\n");
        return;
    }

    std::string token = auth.substr(7);
    size_t p1 = token.find('_');
    size_t p2 = token.find('_', p1 + 1);
    if (p1 == std::string::npos || p2 == std::string::npos) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kUnauthorized);
        request.SendString("Invalid token format\n");
        return;
    }

    try {
        int uid = std::stoi(token.substr(p1 + 1, p2 - p1 - 1));
        ctx.SetData("user_id", uid);
    } catch (...) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kUnauthorized);
        request.SendString("Invalid user ID in token\n");
    }
}

}