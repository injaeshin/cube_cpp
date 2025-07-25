#ifndef HANDLER_MANAGER_HPP
#define HANDLER_MANAGER_HPP

#include "core/core_pch.hpp"
#include "protocol/packet_pch.hpp"
#include "game/handlers/handler_factory.hpp"
#include "core/dispatcher/packet_execute_queue.hpp"
#include "core/session/session_manager.hpp"

namespace game {

// 전방 선언
class EchoHandler;

class HandlerManager {
public:
    // 생성자 및 소멸자
    HandlerManager(std::shared_ptr<core::SessionManager> sessionManager);
    ~HandlerManager() { Stop(); }

    // 생명주기 관리
    void Start() { _executeQueue.Start(); }
    void Stop() { _executeQueue.Stop(); }

private:
    void RegisterAllHandlers();
    void ProcessPacket(protocol::PacketID packetId, const uint64_t sessionId,
                       const uint8_t* payload, size_t size);

private:
    // 패킷 처리 큐
    core::PacketExecuteQueue _executeQueue;
    // 패킷 ID -> 핸들러 함수 매핑
    std::unordered_map<uint16_t, HandlerFunction> _handlers;
    // 세션 관리
    std::shared_ptr<core::SessionManager> _sessionManager;
};

}  // namespace game
#endif  // HANDLER_MANAGER_HPP