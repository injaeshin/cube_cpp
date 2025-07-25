#ifndef IPACKET_DISPATCHER_HPP
#define IPACKET_DISPATCHER_HPP

#include "core/session/isession.hpp"
#include "protocol/packet.hpp"

namespace core {

using PacketHandlerFunc = std::function<void(ISession&, const uint8_t*, size_t)>;

class IPacketDispatcher {
public:
    virtual ~IPacketDispatcher() noexcept = default;

    virtual void Register(protocol::PacketID id, PacketHandlerFunc handler) = 0;
    virtual bool Dispatch(ISession& session, protocol::PacketID id, const uint8_t* data,
                          size_t size) const = 0;
};

}
#endif
