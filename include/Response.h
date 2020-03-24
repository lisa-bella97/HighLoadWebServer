#ifndef RESPONSE_H
#define RESPONSE_H

#include "Header.h"
#include "boost/asio.hpp"
#include <vector>
#include <iostream>
#include <unordered_map>

class Response : public std::enable_shared_from_this<Response> {
public:
    static std::string startProcessing(std::string &, std::string &, std::string &, char &);

    static std::string processMethod(std::string &, std::string &, std::string &, char &, std::vector<Header> &headers);

    static std::string processUnknownMethod();

    static void initHeaders(std::vector<Header> &);

    static void writeHeaders(std::string &, std::string &, std::string &, std::string &, std::vector<Header> &);

public:
    static std::string getDate();

    static std::string getContentType(std::string &);
};

#endif //RESPONSE_H
