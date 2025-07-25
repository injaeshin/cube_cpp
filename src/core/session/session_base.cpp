#include "core/session/session_base.hpp"
#include "protocol/packet.hpp"

namespace core {

SessionBase::SessionBase(asio::ip::tcp::socket&& socket, const uint64_t sessionId)
    : _socket(std::move(socket)), _sessionId(sessionId) {}

void SessionBase::Start() { ReadAsync(); }

void SessionBase::Send(const std::vector<uint8_t>& data) {
    _sendQueue.enqueue(data);

    bool expected = false;
    if (_isWriting.compare_exchange_strong(expected, true)) {
        WriteAsync();
    }
}

void SessionBase::WriteAsync() {
    auto self = shared_from_this();

    std::vector<uint8_t> packet;
    if (_sendQueue.try_dequeue(packet)) {
        _isWriting = false;
        return;
    }

    asio::async_write(_socket, asio::buffer(packet),
                      [this, self](const asio::error_code& ec, std::size_t) {
                          if (ec) {
                              OnError(ec.message());
                              Disconnect();
                              return;
                          }

                          WriteAsync();
                      });
}

void SessionBase::OnError(const std::string& message) {
    std::cout << "SessionBase::OnError: " << message << std::endl;
}

void SessionBase::OnConnected() { std::cout << "SessionBase::OnConnected" << std::endl; }

void SessionBase::OnDisconnected() { std::cout << "SessionBase::OnDisconnected" << std::endl; }

void SessionBase::OnReceived(const uint8_t* data, const size_t size) {
    using namespace protocol;

    if (size < sizeof(PacketHeader)) return;

    const auto* header = reinterpret_cast<const PacketHeader*>(data);
    const auto* payload = data + sizeof(PacketHeader);
    size_t payloadSize = size - sizeof(PacketHeader);
}

void SessionBase::ReadAsync() { std::cout << "SessionBase::ReadAsync" << std::endl; }

}  // namespace core