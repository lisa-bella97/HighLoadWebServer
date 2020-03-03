#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <memory>

#include "Server.h"


/*Server::Server(const ServerConfig &config) : signal_(io_service_, SIGCHLD),
                                             acceptor_(io_service_, tcp::endpoint(tcp::v4(), config.port)),
                                             socket_(io_service_),
                                             document_root_(config.document_root),
                                             cpu_limit_(config.cpu_limit) {
    acceptor_.set_option(tcp::acceptor::reuse_address(true));
    acceptor_.listen();

    serverListen();

    start_signal_wait();
    start_accept();
}

Server::~Server() {
    acceptor_.cancel();
    acceptor_.close();
}

void Server::run() {
    io_service_.run();
}

void Server::serverListen() {
    data_ = boost::array<char, 1024>();
    acceptor_.async_accept(
            socket_,
            boost::bind(&Server::listenHandler,
                        this,
                        boost::asio::placeholders::error
            )
    );
}

void Server::listenHandler(boost::system::error_code error) {
    if (!error) {
        startProcessing();
    } else {
        std::cout << "Failed to accept new connection: " << error.message() << " " << error.value() << std::endl;
    }

    serverListen(); // not a recursion!
}

void Server::startProcessing() {
    socket_.async_read_some(
            boost::asio::buffer(data_),
            boost::bind(
                    &Server::readHandler,
                    shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred
            ));
}

void Server::readHandler(boost::system::error_code error, unsigned int bytes_transferred) {
    if (error) {
        if (bytes_transferred == 0) {
            return;
        }
        std::cout << error.message() << " || " << error.value() << " || " << std::endl;
        return;
    } else {
        std::string method;
        std::string uri;
        char version;
        std::vector<header> headers;
        bool ok = request_->parseRequest(data_.elems, method, uri, version, headers);
        if (ok) {
            std::string response_buffer = response_->startProcessing(method, document_root_, uri, version);
            writeHandler(response_buffer);
        } else {
            return;
        }
    }
}


void Server::writeHandler(std::string& buf) {
    boost::asio::async_write(
            socket_,
            boost::asio::buffer(buf),
            [this](const boost::system::error_code& error, size_t bytes_transferred) {
                if (error) {
                    std::cout << error.message() << " || " << error.value() << " || " << std::endl;
                }
            }
    );
}

void Server::start_signal_wait() {
    signal_.async_wait(boost::bind(&Server::handle_signal_wait, this));
}

void Server::handle_signal_wait() {
    // Only the parent process should check for this signal. We can determine
    // whether we are in the parent by checking if the acceptor is still open.
    if (acceptor_.is_open()) {
        // Reap completed child processes so that we don't end up with zombies.
        int status = 0;
        while (waitpid(-1, &status, WNOHANG) > 0) {}

        start_signal_wait();
    }
}

void Server::start_accept() {
    acceptor_.async_accept(socket_,
                           boost::bind(&Server::handle_accept, this, _1));
}

void Server::handle_accept(const boost::system::error_code &ec) {
    if (!ec) {
        // Inform the io_service that we are about to fork. The io_service cleans
        // up any internal resources, such as threads, that may interfere with
        // forking.
        io_service_.notify_fork(boost::asio::io_service::fork_prepare);

        if (fork() == 0) {
            // Inform the io_service that the fork is finished and that this is the
            // child process. The io_service uses this opportunity to create any
            // internal file descriptors that must be private to the new process.
            io_service_.notify_fork(boost::asio::io_service::fork_child);

            // The child won't be accepting new connections, so we can close the
            // acceptor. It remains open in the parent.
            acceptor_.close();

            // The child process is not interested in processing the SIGCHLD signal.
            signal_.cancel();

            start_read();
        } else {
            // Inform the io_service that the fork is finished (or failed) and that
            // this is the parent process. The io_service uses this opportunity to
            // recreate any internal resources that were cleaned up during
            // preparation for the fork.
            io_service_.notify_fork(boost::asio::io_service::fork_parent);

            socket_.close();
            start_accept();
        }
    } else {
        std::cerr << "Accept error: " << ec.message() << std::endl;
        start_accept();
    }
}

void Server::start_read() {
    socket_.async_read_some(boost::asio::buffer(data_),
                            boost::bind(&Server::handle_read, this, _1, _2));
}

void Server::handle_read(const boost::system::error_code &ec, std::size_t length) {
    if (!ec)
        start_write(length);
}

void Server::start_write(std::size_t length) {
    boost::asio::async_write(socket_, boost::asio::buffer(data_, length),
                             boost::bind(&Server::handle_write, this, _1));
}

void Server::handle_write(const boost::system::error_code &ec) {
    if (!ec)
        start_read();
}
*/