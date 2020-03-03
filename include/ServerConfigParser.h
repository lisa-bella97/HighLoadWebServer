#ifndef SERVERCONFIGPARSER_H
#define SERVERCONFIGPARSER_H


#include <string>

struct ServerConfig {
    unsigned int cpu_limit;
    std::string document_root;
    unsigned int port;
};

class ServerConfigParser {
public:
    static ServerConfig parse(const std::string &path);

private:
    static unsigned int cpu_limit;
    static std::string document_root;
    static unsigned int port;
};


#endif //SERVERCONFIGPARSER_H
