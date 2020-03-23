#ifndef HTTP_SERVER_REQUEST_H
#define HTTP_SERVER_REQUEST_H

#include "header.h"
#include "boost/asio.hpp"
#include <iostream>


class HTTPRequest : public std::enable_shared_from_this<HTTPRequest> {
public:
    explicit HTTPRequest() = default;

    ~HTTPRequest() = default;

    static bool parseRequest(char[], std::string &, std::string &, char &, std::vector<header> &);

    static bool isLetter(char &);

    static bool isDigit(char &);

    static bool isAllowedSymbol(char &);

    static bool isHttpSlash(const char *, size_t &);

    static char decodeChar(const char *, size_t &);
};

#endif //HTTP_SERVER_REQUEST_H
