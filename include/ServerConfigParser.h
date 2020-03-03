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
    static void setPort(unsigned int port);

private:
    static unsigned int cpu_limit_;
    static std::string document_root_;
    static unsigned int port_;
};


#endif //SERVERCONFIGPARSER_H
