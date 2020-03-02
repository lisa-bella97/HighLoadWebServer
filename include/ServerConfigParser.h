#ifndef SERVERCONFIGPARSER_H
#define SERVERCONFIGPARSER_H


#include <string>

struct config {
    int cpu_limit;
    std::string document_root;
};

class ServerConfigParser {
public:
    static config parse(const std::string &path);

private:
    static int cpu_limit;
    static std::string document_root;
};


#endif //SERVERCONFIGPARSER_H
