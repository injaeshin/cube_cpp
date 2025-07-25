#ifndef PACKET_EXECUTE_QUEUE_HPP
#define PACKET_EXECUTE_QUEUE_HPP

#include "core/core_pch.hpp"
#include "core/common/queue_adapter.hpp"
#include "protocol/packet.hpp"

namespace core {

using PacketProcessorFunc =
    std::function<void(protocol::PacketID, const uint64_t, const uint8_t*, size_t)>;

class ISession;

class PacketExecuteQueue {
public:
    PacketExecuteQueue();
    ~PacketExecuteQueue();

    void SetProcessor(PacketProcessorFunc processor);
    void Enqueue(protocol::PacketItem item);
    bool TryDequeue(protocol::PacketItem& item);
    bool HasPending() const;

    void Start();
    void Stop();

private:
    void Run();
    void ProcessPackets();

private:
    std::atomic<bool> _isRunning;
    std::thread _thread;
    std::mutex _mutex;
    std::condition_variable _cv;

    ConcurrentQueue<protocol::PacketItem> _queue;
    PacketProcessorFunc _processor;
};

}  // namespace core
#endif  // PACKET_EXECUTE_QUEUE_HPP