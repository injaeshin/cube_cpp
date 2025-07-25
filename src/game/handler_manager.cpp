#include "game/handler_manager.hpp"
#include "game/handlers/echo_handler.hpp"
#include "protocol/generated/echo_generated.h"

namespace game {

HandlerManager::HandlerManager(std::shared_ptr<core::SessionManager> sessionManager)
    : _sessionManager(sessionManager) {
    RegisterAllHandlers();
    _executeQueue.SetProcessor(
        [this](const protocol::PacketID packetId, const uint64_t sessionId, const uint8_t* payload,
               const size_t size) { ProcessPacket(packetId, sessionId, payload, size); });
}

void HandlerManager::RegisterAllHandlers() {
    // Echo 핸들러 등록 - 핸들러 팩토리 사용
    _handlers[static_cast<uint16_t>(protocol::PacketID::REQ_ECHO)] =
        HandlerFactory::CreateEchoHandler();

    // 여기에 추가 핸들러 등록
    // 예: LoginHandler, MovementHandler 등
}

void HandlerManager::ProcessPacket(protocol::PacketID packetId, const uint64_t sessionId,
                                   const uint8_t* payload, size_t size) {
    auto session = _sessionManager->FindBySessionId(sessionId);
    if (!session) {
        return;
    }

    auto it = _handlers.find(static_cast<uint16_t>(packetId));
    if (it == _handlers.end()) {
        return;
    }

    it->second(*session, payload, size);
}

}  // namespace game
