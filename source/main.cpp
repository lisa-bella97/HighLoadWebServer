/*#include <iostream>
#include <ServerConfigParser.h>
#include <Server.h>

int main() {
    auto server_config = ServerConfigParser::parse("../httpd.conf");
    std::cout << "document_root = " << server_config.document_root << std::endl <<
              "cpu_limit = " << server_config.cpu_limit << std::endl;

    try {
        Server s(server_config);
        s.run();
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}*/

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/write.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <cstdlib>
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>
#include <ServerConfigParser.h>

#include "request.h"
#include "response.h"

using boost::asio::ip::tcp;

class server {
public:
    server(boost::asio::io_service &io_service, const ServerConfig &config)
            : io_service_(io_service),
              signal_(io_service, SIGCHLD),
              acceptor_(io_service, tcp::endpoint(tcp::v4(), config.port)),
              socket_(io_service),
              document_root_(config.document_root) {
        start_signal_wait();
        start_accept();
    }

private:
    void start_signal_wait() {
        signal_.async_wait(boost::bind(&server::handle_signal_wait, this));
    }

    void handle_signal_wait() {
        // Only the parent process should check for this signal. We can determine
        // whether we are in the parent by checking if the acceptor is still open.
        if (acceptor_.is_open()) {
            // Reap completed child processes so that we don't end up with zombies.
            int status = 0;
            while (waitpid(-1, &status, WNOHANG) > 0) {}

            start_signal_wait();
        }
    }

    void start_accept() {
        acceptor_.async_accept(socket_,
                               boost::bind(&server::handle_accept, this, _1));
    }

    void handle_accept(const boost::system::error_code &ec) {
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

    void start_read() {
        std::cout << "start read!" << std::endl;
        socket_.async_read_some(boost::asio::buffer(data_),
                                boost::bind(&server::handle_read, this, _1, _2));
    }

    void handle_read(const boost::system::error_code &ec, std::size_t length) {
        if (ec) {
            if (length == 0) {
                return;
            }
            std::cout << ec.message() << " || " << ec.value() << " || " << std::endl;
            return;
        } else {
            std::string method;
            std::string uri;
            char version;
            std::vector<header> headers;
            bool ok = request_->parseRequest(data_.elems, method, uri, version, headers);
            if (ok) {
                std::string response_buffer = response_->startProcessing(method, document_root_, uri, version);
                start_write(response_buffer);
            } else {
                std::cout << "cannot parse request" << std::endl;
                return;
            }
        }
        //if (!ec)
        //start_write(length);
    }

    void start_write(std::string &buf) {
        std::cout << "start write!" << std::endl;
        boost::asio::async_write(socket_, boost::asio::buffer(buf),
                                 [this](const boost::system::error_code &error, size_t bytes_transferred) {
                                     if (error) {
                                         std::cout << error.message() << " || " << error.value() << " || " << std::endl;
                                     }
                                 });

        //if (!ec)
        //start_read();
    }

    boost::asio::io_service &io_service_;
    boost::asio::signal_set signal_;
    tcp::acceptor acceptor_;
    tcp::socket socket_;
    boost::array<char, 1024> data_;

    std::string document_root_;
    std::shared_ptr<HTTPRequest> request_;
    std::shared_ptr<HTTPResponse> response_;
};

int main() {
    try {
        auto server_config = ServerConfigParser::parse("../httpd.conf");
        std::cout << "document_root = " << server_config.document_root << std::endl <<
                  "cpu_limit = " << server_config.cpu_limit << std::endl;

        boost::asio::io_service io_service;

        server s(io_service, server_config);

        io_service.run();
    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}
