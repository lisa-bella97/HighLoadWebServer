#include "Server.h"

Server::Server(const ServerConfig &config) : signal_(io_service_, SIGCHLD),
                                             acceptor_(io_service_, tcp::endpoint(tcp::v4(), config.port)),
                                             socket_(io_service_) {
    start_signal_wait();
    start_accept();
}

void Server::run() {
    io_service_.run();
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
