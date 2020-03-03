#ifndef SERVER_H
#define SERVER_H


#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/write.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <cstdlib>
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "ServerConfigParser.h"

using boost::asio::ip::tcp;

class Server {
public:
    explicit Server(const ServerConfig& config);
    void run();

private:
    void start_signal_wait();
    void handle_signal_wait();
    void start_accept();
    void handle_accept(const boost::system::error_code& ec);
    void start_read();
    void handle_read(const boost::system::error_code& ec, std::size_t length);
    void start_write(std::size_t length);
    void handle_write(const boost::system::error_code& ec);

    boost::asio::io_service io_service_;
    boost::asio::signal_set signal_;
    tcp::acceptor acceptor_;
    tcp::socket socket_;
    boost::array<char, 1024> data_;
};


#endif //SERVER_H
