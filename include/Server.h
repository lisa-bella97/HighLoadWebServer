#ifndef SERVER_H
#define SERVER_H

#include "ServerConfig.h"
#include "ServerConnection.h"
#include "boost/asio.hpp"

class Server {
public:
    explicit Server(ServerConfig *config);

    ~Server();

    void run();

private:
    void startAccept();

    void handleAccept(const boost::system::error_code& ec);

    void startSignalWait();

    void handleSignalWait();

    boost::asio::io_service io_service_;
    boost::asio::signal_set signal_;
    boost::asio::ip::tcp::acceptor acceptor_;
    std::shared_ptr<ServerConnection> connection_;
    std::string document_root_;
};

#endif //SERVER_H
