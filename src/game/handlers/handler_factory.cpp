
#include "game/handlers/echo_handler.hpp"
#include "game/handlers/handler_factory.hpp"
#include "protocol/generated/echo_generated.h"

namespace game {

HandlerFunction HandlerFactory::CreateEchoHandler() {
    auto handler = std::make_shared<EchoHandler>();
    return [handler](core::ISession& session, const uint8_t* payload, size_t size) {
        const auto* request = flatbuffers::GetRoot<protocol::EchoRequest>(payload);
        handler->Handle(session, request);
    };
}

// 추가 핸들러 생성 메서드 구현
// HandlerFunction HandlerFactory::CreateLoginHandler() { ... }
// HandlerFunction HandlerFactory::CreateMovementHandler() { ... }

} // namespace game
