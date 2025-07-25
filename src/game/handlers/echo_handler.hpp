#ifndef ECHO_HANDLER_HPP
#define ECHO_HANDLER_HPP

#include "core/core_pch.hpp"
#include "core/session/isession.hpp"
#include "protocol/generated/echo_generated.h"

namespace game {

class EchoHandler {
public:
    void Handle(core::ISession& session, const protocol::EchoRequest* request);
};

}  // namespace game
#endif  // ECHO_HANDLER_HPP
