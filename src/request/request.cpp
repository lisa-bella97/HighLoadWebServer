#define STAGE_METHOD 0
#define STAGE_SLASH 1
#define STAGE_URI 2
#define STAGE_HTTP 3
#define STAGE_HTTP_VERSION 4
#define STAGE_R 5
#define STAGE_N 6
#define STAGE_HEADER_KEY 7
#define STAGE_HEADER_DELIM 8
#define STAGE_HEADER_VALUE 9

#include "request.h"
#include <regex>
#include <chrono>
#include <thread>

bool HTTPRequest::parseRequest(char *request, std::string &method, std::string &uri, char &version,
                               std::vector<header> &headers) {
    size_t i = STAGE_METHOD, j = 0; // go to method phase
    header tmp_struct{"", ""};

    while (request[j] != '\0') {
        switch (i) {
            case STAGE_METHOD: {
                if (isLetter(request[j])) {
                    method.push_back(request[j++]);
                } else if (request[j] == ' ') {
                    ++j;
                    i = STAGE_SLASH; // go to '/' phase
                } else {
                    return false;
                }
                break;
            }

            case STAGE_SLASH: {
                if (request[j] == '/') {
                    uri.push_back(request[j++]);
                    i = STAGE_URI; // go to uri phase
                } else {
                    return false;
                }
                break;
            }

            case STAGE_URI: {
                if (isLetter(request[j]) || isAllowedSymbol(request[j]) || isDigit(request[j])) {
                    uri.push_back(request[j++]);
                } else if (request[j] == '%' || request[j] == '+') {
                    uri.push_back(decodeChar(request, j));
                    j += 3;
                } else if (request[j] == ' ') {
                    ++j;
                    i = STAGE_HTTP; // go to http/ phase
                }
                break;
            }

            case STAGE_HTTP: {
                if (isHttpSlash(request, j)) {
                    j += 5;
                    i = STAGE_HTTP_VERSION; // go to http 1.[0 or 1] phase
                } else {
                    return false;
                }
                break;
            };

            case STAGE_HTTP_VERSION: {
                if (isDigit(request[j]) && request[j + 1] == '.' && isDigit(request[j + 2])) {
                    version = request[2 + j];
                    j += 3;
                    i = STAGE_R; // go to \r phase
                } else {
                    return false;
                }
                break;
            }

            case STAGE_R: {
                if (request[j] == '\r') {
                    ++j;
                    i = STAGE_N; // go to \n phase
                } else {
                    return false;
                }
                break;
            }

            case STAGE_N: {
                if (request[j] == '\n') {
                    if (request[j + 1] == '\0') {
                        if (request[j - 2] == '\n') {
                            ++j;
                        } else {
                            return false;
                        }
                    } else if (request[j + 1] == '\r') {
                        i = STAGE_R; // go to \r phase
                    } else {
                        i = STAGE_HEADER_KEY; // go to header key phase
                    }
                    ++j;
                } else {
                    return false;
                }
                break;
            }

            case STAGE_HEADER_KEY: {
                if (isLetter(request[j]) || request[j] == '-' || isDigit(request[j])) {
                    tmp_struct.key.push_back(request[j++]);
                    if (request[j] == ':') {
                        i = STAGE_HEADER_DELIM; // go to : and space phase
                    }
                } else {
                    return false;
                }
                break;
            }

            case STAGE_HEADER_DELIM: {
                if (request[j] == ':' && request[j + 1] == ' ') {
                    j += 2;
                    i = STAGE_HEADER_VALUE; //go to header value phase
                } else {
                    return false;
                }
                break;
            }

            case STAGE_HEADER_VALUE: {
                tmp_struct.value.push_back(request[j++]);

                if (request[j] == '\r') {
                    headers.push_back(tmp_struct);

                    tmp_struct.key = "";
                    tmp_struct.value = "";

                    i = STAGE_R; // go to \r phase
                }
                break;
            }
        }
    }

    return true;
}

bool HTTPRequest::isLetter(char &c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool HTTPRequest::isDigit(char &c) {
    return (c >= '0' && c <= '9');
}

bool HTTPRequest::isAllowedSymbol(char &c) {
    return (c == '.' || c == '/' || c == '?' || c == '&' || c == '=' || c == '_' || c == '-');
}

bool HTTPRequest::isHttpSlash(const char *request, size_t &i) {
    return (
            request[i] == 'H' &&
            request[i + 1] == 'T' &&
            request[i + 2] == 'T' &&
            request[i + 3] == 'P' &&
            request[i + 4] == '/'
    );
}

char HTTPRequest::decodeChar(const char *request, size_t &i) {
    if (request[i] == '%') {
        char str[3];
        memcpy(str, &request[i + 1], 2);

        std::stringstream ss;
        ss << std::hex << str;
        size_t ch = 0;
        ss >> ch;

        return static_cast<char>(ch);
    } else if (request[i] == '+') {
        return ' ';
    }
}
