#include "core/core_pch.hpp"
#include "core/core_asio_pch.hpp"
#include "core/network/tcp_acceptor.hpp"
#include "core/common/service_registry.hpp"
#include "core/session/session_manager.hpp"
#include "game/handler_manager.hpp"

// 서비스 등록
void RegisterService(core::ServiceRegistry& serviceRegistry) {
    const auto sessionManager = std::make_shared<core::SessionManager>();
    serviceRegistry.Registry<core::SessionManager>(sessionManager);

    const auto handlerManager = std::make_shared<game::HandlerManager>(sessionManager);
    serviceRegistry.Registry<game::HandlerManager>(handlerManager);

    handlerManager->Start();
}

int main() {
    core::ServiceRegistry serviceRegistry;
    RegisterService(serviceRegistry);

    return 0;
}
