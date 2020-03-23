#ifndef HTTP_SERVER_SERVER_H
#define HTTP_SERVER_SERVER_H

#include "../common/config.h"
#include "../connection/connection.h"
#include "boost/asio.hpp"
#include "boost/thread.hpp"

class HTTPServer : public std::enable_shared_from_this<HTTPServer> {
public:
    explicit HTTPServer(HTTPConfig *cfg);

    ~HTTPServer();

    void serverStart();

private:
    void serverListen();

    void serverStop();

    void listenHandler(boost::system::error_code);

    void start_signal_wait();

    void handle_signal_wait();

private:
    boost::asio::io_service io_service_;
    boost::asio::signal_set signal_;
    boost::asio::ip::tcp::acceptor acceptor_;
    boost::thread_group thread_group_;
    std::shared_ptr<HTTPConnection> connection_;
    std::string document_root;
    size_t cpu_limit;
    size_t thread_limit;
};

#endif //HTTP_SERVER_SERVER_H
