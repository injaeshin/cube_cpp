#ifndef HANDLER_FACTORY_HPP
#define HANDLER_FACTORY_HPP

#include "core/core_pch.hpp"
#include "core/session/isession.hpp"

namespace game {

using HandlerFunction = std::function<void(core::ISession&, const uint8_t*, size_t)>;

// 모든 패킷 핸들러를 생성하는 팩토리 클래스
class HandlerFactory {
public:
    // 에코 패킷 핸들러 생성
    static HandlerFunction CreateEchoHandler();

    // 추가 핸들러 생성 메서드들
    // static HandlerFunction CreateLoginHandler();
    // static HandlerFunction CreateMovementHandler();
};

} // namespace game

#endif // HANDLER_FACTORY_HPP
