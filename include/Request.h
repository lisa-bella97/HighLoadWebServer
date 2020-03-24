#ifndef REQUEST_H
#define REQUEST_H

#include "Header.h"
#include <vector>

class Request {
public:
    static bool parseRequest(const char *request, std::string &method, std::string &uri, char &version,
                             std::vector<Header> &headers);

private:

    static bool isLetter(char c);

    static bool isDigit(char c);

    static bool isAllowedSymbol(char c);

    static bool isHttpSlash(const char *request, size_t i);

    static char decodeChar(const char *request, size_t i);
};

#endif //REQUEST_H
