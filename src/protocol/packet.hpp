#ifndef PACKET_TYPES_H
#define PACKET_TYPES_H

#include <cstdint>

namespace protocol {

// Packet Type
enum class PacketID : uint16_t {

    None = 0,

    REQ_ECHO = 1000,
    RES_ECHO = 1001,
};

#pragma pack(push, 1)
struct PacketHeader {
    uint16_t length;
    uint16_t id;
};
#pragma pack(pop)

struct PacketItem {
    PacketID packetId;
    uint64_t sessionId;
    const uint8_t* payload;
    size_t payloadSize;

    PacketItem() = default;
    PacketItem(const PacketID id, const uint64_t sessionId, const uint8_t* data, const size_t size)
        : packetId(id), sessionId(sessionId), payload(data), payloadSize(size) {}
};

constexpr size_t PACKET_HEADER_SIZE = sizeof(PacketHeader);

}  // namespace protocol
#endif
