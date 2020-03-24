#ifndef REQUEST_H
#define REQUEST_H

#include "Header.h"
#include "boost/asio.hpp"
#include <iostream>

class Request : public std::enable_shared_from_this<Request> {
public:
    static bool parseRequest(char[], std::string &, std::string &, char &, std::vector<Header> &);

    static bool isLetter(char &);

    static bool isDigit(char &);

    static bool isAllowedSymbol(char &);

    static bool isHttpSlash(const char *, size_t &);

    static char decodeChar(const char *, size_t &);
};

#endif //REQUEST_H
