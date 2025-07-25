#ifndef PACKET_DISPATCHER_HPP
#define PACKET_DISPATCHER_HPP

#include "flatbuffers/buffer.h"
#include "core/dispatcher/packet_dispatcher_interface.hpp"

namespace core {

class PacketDispatcher : public IPacketDispatcher {
public:
    PacketDispatcher() = default;
    void Register(protocol::PacketID id, PacketHandlerFunc handler) override {
        _handlers[static_cast<uint16_t>(id)] = std::move(handler);
    }

    bool Dispatch(ISession& session, protocol::PacketID id, const uint8_t* payload,
                  const size_t size) const override {
        auto it = _handlers.find(static_cast<uint16_t>(id));
        if (it == _handlers.end()) {
            return false;
        }

        it->second(session, payload, size);
        return true;
    }

private:
    std::unordered_map<uint16_t, PacketHandlerFunc> _handlers;
};

}  // namespace core
#endif