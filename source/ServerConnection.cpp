#include "ServerConnection.h"
#include "boost/bind.hpp"
#include <iostream>
#include <thread>

ServerConnection::ServerConnection(boost::asio::io_service &io_service, std::string &root) :
        socket_(io_service),
        document_root_(root),
        buffer_(),
        version_() {};

ServerConnection::~ServerConnection() {
    //socket_.cancel();
    //socket_.close();
}

void ServerConnection::startRead() {
    socket_.async_read_some(
            boost::asio::buffer(buffer_),
            boost::bind(
                    &ServerConnection::handleRead,
                    shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred
            ));
}

boost::asio::ip::tcp::socket &ServerConnection::getSocket() {
    return socket_;
}

void ServerConnection::handleRead(boost::system::error_code ec, size_t length) {
    if (ec) {
        if (length == 0) {
            return;
        }
        std::cout << ec.message() << " || " << ec.value() << " || " << std::endl;
    } else {
        std::cout << buffer_.elems << std::endl;
        if (request_->parseRequest(buffer_.elems, method_, uri_, version_, headers_)) {
            response_buffer = response_->startProcessing(method_, document_root_, uri_, version_);
            auto self_ptr = shared_from_this();
            handleWrite(response_buffer, self_ptr);
        } else {
            std::cout << "Cannot parse request" << std::endl;
            return;
        }
    }
}

void ServerConnection::handleWrite(std::string &buf, std::shared_ptr<ServerConnection> &self) {
    boost::asio::async_write(
            socket_,
            boost::asio::buffer(buf),
            [self](const boost::system::error_code &error, size_t bytes_transferred) {
                if (error) {
                    std::cout << error.message() << " || " << error.value() << " || " << std::endl;
                }
            }
    );
}
