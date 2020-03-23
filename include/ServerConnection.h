#ifndef SERVER_CONNECTION_H
#define SERVER_CONNECTION_H

#include "request.h"
#include "response.h"
#include "boost/asio.hpp"
#include "header.h"
#include <boost/array.hpp>

class ServerConnection : public std::enable_shared_from_this<ServerConnection> {
public:
    explicit ServerConnection(boost::asio::io_service &io_service, std::string &root);

    ~ServerConnection();

    void startRead();

    boost::asio::ip::tcp::socket &getSocket();

private:
    void handleRead(boost::system::error_code ec, size_t length);

    void handleWrite(std::string &buf, std::shared_ptr<ServerConnection> &self);

private:
    boost::asio::ip::tcp::socket socket_;
    boost::array<char, 1024> buffer_;
    std::shared_ptr<HTTPRequest> request_;
    std::shared_ptr<HTTPResponse> response_;

private:
    std::string document_root_;
    std::string method_;
    std::string uri_;
    std::string response_buffer;
    char version_;
    std::vector<header> headers_;
};

#endif //SERVER_CONNECTION_H
