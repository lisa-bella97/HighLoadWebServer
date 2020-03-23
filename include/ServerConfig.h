#ifndef SERVER_CONFIG_H
#define SERVER_CONFIG_H

#include <string>

class ServerConfig {
public:
    explicit ServerConfig(unsigned int port = 80);

    void parse(const std::string &path);

    std::string getDocumentRoot();

    unsigned int getPort();

private:
    unsigned int cpu_limit_;
    unsigned int thread_limit_;
    std::string document_root_;
    unsigned int port_;
};

#endif //SERVER_CONFIG_H
