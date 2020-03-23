#include "connection.h"
#include "boost/bind.hpp"
#include <iostream>
#include <thread>

HTTPConnection::HTTPConnection(boost::asio::io_service &io_service, std::string &root) :
        socket_(io_service),
        document_root_(root),
        buffer_(),
        version_() {};

HTTPConnection::~HTTPConnection() {
    stopProcessing();
}

void HTTPConnection::stopProcessing() {
    //socket_.cancel();
    //socket_.close();
}

void HTTPConnection::startProcessing() {
    socket_.async_read_some(
            boost::asio::buffer(buffer_),
            boost::bind(
                    &HTTPConnection::readHandler,
                    shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred
            ));
}

void HTTPConnection::readHandler(boost::system::error_code error, size_t bytes_transferred) {
    if (error) {

        if (bytes_transferred == 0) {
            return;
        }

        std::cout << error.message() << " || " << error.value() << " || " << std::endl;
        return;
    } else {

        bool ok = request_->parseRequest(buffer_.elems, method_, uri_, version_, headers_);

        if (ok) {
            response_buffer = response_->startProcessing(method_, document_root_, uri_, version_);
            auto self_ptr = shared_from_this();
            writeHandler(response_buffer, self_ptr);
        } else {
            return;
        }
    }
}

void HTTPConnection::writeHandler(std::string &buf, std::shared_ptr<HTTPConnection> &self) {
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
