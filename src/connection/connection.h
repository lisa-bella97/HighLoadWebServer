#ifndef HTTP_SERVER_CONNECTION_H
#define HTTP_SERVER_CONNECTION_H

#define MAX_READ_BUFFER_SIZE 1024

#include "../request/request.h"
#include "../response/response.h"
#include "boost/asio.hpp"
#include <boost/array.hpp>

class HTTPConnection : public std::enable_shared_from_this<HTTPConnection> {
public:
    explicit HTTPConnection(boost::asio::io_service &, std::string &);

    ~HTTPConnection();

    void startProcessing();

    boost::asio::ip::tcp::socket &getSocket() { return socket_; };

private:
    void stopProcessing();

    void readHandler(boost::system::error_code, size_t);

    void writeHandler(std::string &buf, std::shared_ptr<HTTPConnection> &);

private:
    boost::asio::ip::tcp::socket socket_;
    boost::array<char, MAX_READ_BUFFER_SIZE> buffer_;
    std::shared_ptr<HTTPRequest> request_;
    std::shared_ptr<HTTPResponse> response_;

private:
    std::string document_root_;
    std::string method_;
    std::string uri_;
    std::string response_buffer;
    char version_;
    std::vector<header> headers_;
};

#endif //HTTP_SERVER_CONNECTION_H
