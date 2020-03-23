#include "server.h"
#include "boost/bind.hpp"
#include <thread>
#include <wait.h>

HTTPServer::HTTPServer(HTTPConfig *cfg) :
        acceptor_(
                io_service_,
                boost::asio::ip::tcp::endpoint(
                        boost::asio::ip::tcp::v4(),
                        cfg->getPort()
                )),
        signal_(io_service_, SIGCHLD),
        connection_() {
    document_root = cfg->getDocumentRoot();
    cpu_limit = cfg->getCPULimit();
    thread_limit = cfg->getThreadLimit();

    acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    acceptor_.listen();

    start_signal_wait();
    serverListen();
}

HTTPServer::~HTTPServer() {
    serverStop();
}

void HTTPServer::serverStop() {
    acceptor_.close();
}

void HTTPServer::serverStart() {
    io_service_.run();
}

void HTTPServer::start_signal_wait() {
    signal_.async_wait(boost::bind(&HTTPServer::handle_signal_wait, this));
}

void HTTPServer::handle_signal_wait() {
    // Only the parent process should check for this signal. We can determine
    // whether we are in the parent by checking if the acceptor is still open.
    if (acceptor_.is_open()) {
        // Reap completed child processes so that we don't end up with zombies.
        int status = 0;
        while (waitpid(-1, &status, WNOHANG) > 0) {}

        start_signal_wait();
    }
}

void HTTPServer::serverListen() {
    connection_.reset(new HTTPConnection(io_service_, document_root));
    acceptor_.async_accept(
            connection_->getSocket(),
            boost::bind(&HTTPServer::listenHandler,
                        this,
                        boost::asio::placeholders::error
            )
    );
}

void HTTPServer::listenHandler(boost::system::error_code error) {
    if (!error) {
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

            connection_->startProcessing();
        } else {
            // Inform the io_service that the fork is finished (or failed) and that
            // this is the parent process. The io_service uses this opportunity to
            // recreate any internal resources that were cleaned up during
            // preparation for the fork.
            io_service_.notify_fork(boost::asio::io_service::fork_parent);

            //socket_.cancel();
            connection_->getSocket().close();
            serverListen();
        }
    } else {
        std::cerr << "Accept error: " << error.message() << std::endl;
        serverListen();
    }
}
