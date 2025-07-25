#include "core/network/tcp_acceptor.hpp"

namespace core {

TcpAcceptor::TcpAcceptor(asio::io_context& io_context, uint16_t port,
                         const SessionFactory& sessionFactory)
    : _acceptor(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)),
      _io_context(io_context),
      _sessionFactory(sessionFactory) {}

void TcpAcceptor::Start() {
    _isRunning = true;
    DoAccept();
}

void TcpAcceptor::Stop() {
    _isRunning = false;
    asio::error_code ec;
    _acceptor.close(ec);
}

void TcpAcceptor::DoAccept() {
    if (!_isRunning) return;

    _acceptor.async_accept([this](const asio::error_code& ec, asio::ip::tcp::socket socket) {
        if (ec) return;

        if (_sessionFactory) {
            _sessionFactory(std::move(socket));
        }

        DoAccept();
    });
}

}  // namespace core