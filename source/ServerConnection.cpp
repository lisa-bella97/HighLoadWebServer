#include "ServerConnection.h"
#include "boost/bind.hpp"
#include <iostream>
#include <thread>

ServerConnection::ServerConnection(boost::asio::io_service &io_service, std::string &root) :
        socket_(io_service),
        document_root_(root),
        buffer_(),
        version_() {}

void ServerConnection::startRead() {
    socket_.async_read_some(
            boost::asio::buffer(buffer_),
            boost::bind(&ServerConnection::handleRead, this, _1, _2));
}

boost::asio::ip::tcp::socket &ServerConnection::getSocket() {
    return socket_;
}

void ServerConnection::handleRead(const boost::system::error_code& ec, std::size_t length) {
    if (ec) {
        if (length == 0) {
            return;
        }
        std::cout << ec.message() << " || " << ec.value() << " || " << std::endl;
    } else {
        if (request_->parseRequest(buffer_.elems, method_, uri_, version_, headers_)) {
            response_buffer = response_->startProcessing(method_, document_root_, uri_, version_);
            handleWrite(response_buffer);
        } else {
            return;
        }
    }
}

void ServerConnection::handleWrite(std::string &buf) {
    boost::asio::async_write(
            socket_,
            boost::asio::buffer(buf),
            [this](const boost::system::error_code &error, size_t bytes_transferred) {
                if (error) {
                    std::cout << error.message() << " || " << error.value() << " || " << std::endl;
                }
            }
    );
}
