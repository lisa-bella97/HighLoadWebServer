#ifndef SERVER_CONNECTION_H
#define SERVER_CONNECTION_H

#include "Request.h"
#include "Response.h"
#include "Header.h"
#include "boost/asio.hpp"
#include <boost/array.hpp>

class ServerConnection {
public:
    explicit ServerConnection(boost::asio::io_service &io_service, std::string &root);

    void startRead();

    boost::asio::ip::tcp::socket &getSocket();

private:
    void handleRead(const boost::system::error_code& ec, std::size_t length);

    void handleWrite(std::string &buf);

    boost::asio::ip::tcp::socket socket_;
    boost::array<char, 1024> buffer_;

    std::string document_root_;
    std::string response_buffer_;
};

#endif //SERVER_CONNECTION_H
