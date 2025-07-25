#include "core/dispatcher/packet_execute_queue.hpp"

namespace core {

PacketExecuteQueue::PacketExecuteQueue() : _isRunning(false) {}

PacketExecuteQueue::~PacketExecuteQueue() { Stop(); }

void PacketExecuteQueue::SetProcessor(PacketProcessorFunc processor) {
    _processor = std::move(processor);
}

void PacketExecuteQueue::Enqueue(protocol::PacketItem item) {
    _queue.enqueue(std::move(item));
    _cv.notify_one();
}

bool PacketExecuteQueue::TryDequeue(protocol::PacketItem& item) { return _queue.try_dequeue(item); }

bool PacketExecuteQueue::HasPending() const { return _queue.size_approx() > 0; }

void PacketExecuteQueue::Start() {
    _isRunning = true;
    _thread = std::thread([this]() { Run(); });
}

void PacketExecuteQueue::Stop() {
    {
        std::lock_guard lock(_mutex);
        _isRunning = false;
    }

    _cv.notify_all();
    if (_thread.joinable()) {
        _thread.join();
    }
}

void PacketExecuteQueue::Run() {
    while (true) {
        std::unique_lock lock(_mutex);
        _cv.wait(lock, [this] { return !_isRunning || HasPending(); });

        if (!_isRunning && !HasPending()) return;

        lock.unlock();
        ProcessPackets();
    }
}

void PacketExecuteQueue::ProcessPackets() {
    while (HasPending()) {
        protocol::PacketItem item{};
        if (TryDequeue(item)) {
            try {
                if (_processor) {
                    _processor(item.packetId, item.sessionId, item.payload, item.payloadSize);
                }
            } catch (const std::exception& e) {
                // 예외를 로그하지만 worker는 계속 동작
                // 실제 프로덕션에서는 로깅 시스템 사용
            } catch (...) {
                // 알 수 없는 예외도 처리
            }
        }
    }
}

}  // namespace core
