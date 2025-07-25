#ifndef TCP_ACCEPTOR_HPP
#define TCP_ACCEPTOR_HPP

#include "core/core_pch.hpp"
#include "core/core_asio_pch.hpp"

namespace core {

using SessionFactory = std::function<void(asio::ip::tcp::socket&&)>;

class TcpAcceptor {
public:
    TcpAcceptor(asio::io_context& io_context, uint16_t port, const SessionFactory& sessionFactory);

    void Start();
    void Stop();

private:
    void DoAccept();

private:
    asio::ip::tcp::acceptor _acceptor;
    asio::io_context& _io_context;
    SessionFactory _sessionFactory;
    bool _isRunning = false;
};

}
#endif