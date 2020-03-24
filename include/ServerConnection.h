#ifndef SERVER_CONNECTION_H
#define SERVER_CONNECTION_H

#include "Request.h"
#include "Response.h"
#include "boost/asio.hpp"
#include "Header.h"
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
    std::shared_ptr<Request> request_;
    std::shared_ptr<Response> response_;

    std::string document_root_;
    std::string method_;
    std::string uri_;
    std::string response_buffer;
    char version_;
    std::vector<Header> headers_;
};

#endif //SERVER_CONNECTION_H
