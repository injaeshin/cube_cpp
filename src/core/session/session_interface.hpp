#ifndef ISESSION_HPP
#define ISESSION_HPP

#include "core/core_pch.hpp"

namespace core {

class ISession {
public:
    virtual ~ISession() noexcept = default;

    virtual void Send(const std::vector<uint8_t>& data) = 0;

    virtual void OnConnected() = 0;
    virtual void OnDisconnected() = 0;
    virtual void OnReceived(const uint8_t* data, size_t size) = 0;
    virtual void OnError(const std::string& message) = 0;
};

}
#endif