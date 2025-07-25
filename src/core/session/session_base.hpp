#ifndef SESSION_BASE_HPP
#define SESSION_BASE_HPP

#include "core/core_asio_pch.hpp"
#include "core/core_pch.hpp"
#include "core/session/session_interface.hpp"
#include "core/buffer/ring_buffer.hpp"
#include "core/common/queue_adapter.hpp"

namespace core {

class SessionBase final : public ISession, public std::enable_shared_from_this<SessionBase> {
public:
    explicit SessionBase(asio::ip::tcp::socket&& socket, uint64_t sessionId);
    ~SessionBase() noexcept override = default;

    void Start();
    void Disconnect();

    void Send(const std::vector<uint8_t>& data) override;

    uint64_t GetSessionId() const { return _sessionId; }

protected:
    virtual void OnError(const std::string& message) override;
    virtual void OnConnected() override;
    virtual void OnDisconnected() override;
    virtual void OnReceived(const uint8_t* data, size_t size) override;

private:
    void ReadAsync();
    void WriteAsync();

    asio::ip::tcp::socket _socket;
    RingBuffer _recvBuffer;

    ConcurrentQueue<std::vector<uint8_t>> _sendQueue;
    std::atomic<bool> _isWriting = false;

    uint64_t _sessionId;
};

}  // namespace core
#endif