#include "Server.h"
#include "boost/bind.hpp"
#include <iostream>
#include <wait.h>

Server::Server(ServerConfig *config) :
        acceptor_(
                io_service_,
                boost::asio::ip::tcp::endpoint(
                        boost::asio::ip::tcp::v4(),
                        config->getPort()
                )),
        signal_(io_service_, SIGCHLD),
        connection_(),
        document_root_(config->getDocumentRoot()) {
    startSignalWait();
    startAccept();
}

Server::~Server() {
    acceptor_.close();
}

void Server::run() {
    io_service_.run();
}

void Server::startSignalWait() {
    signal_.async_wait(boost::bind(&Server::handleSignalWait, this));
}

void Server::handleSignalWait() {
    // Only the parent process should check for this signal. We can determine
    // whether we are in the parent by checking if the acceptor is still open.
    if (acceptor_.is_open()) {
        // Reap completed child processes so that we don't end up with zombies.
        int status = 0;
        while (waitpid(-1, &status, WNOHANG) > 0) {}

        startSignalWait();
    }
}

void Server::startAccept() {
    connection_.reset(new ServerConnection(io_service_, document_root_));
    acceptor_.async_accept(
            connection_->getSocket(),
            boost::bind(&Server::handleAccept, this, _1)
    );
}

void Server::handleAccept(const boost::system::error_code &ec) {
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
            //acceptor_.cancel();
            acceptor_.close();

            // The child process is not interested in processing the SIGCHLD signal.
            signal_.cancel();

            connection_->startRead();
        } else {
            // Inform the io_service that the fork is finished (or failed) and that
            // this is the parent process. The io_service uses this opportunity to
            // recreate any internal resources that were cleaned up during
            // preparation for the fork.
            io_service_.notify_fork(boost::asio::io_service::fork_parent);

            //socket_.cancel();
            connection_->getSocket().close();
            startAccept();
        }
    } else {
        std::cerr << "Accept error: " << ec.message() << std::endl;
        startAccept();
    }
}
