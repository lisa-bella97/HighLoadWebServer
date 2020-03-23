#ifndef HTTP_SERVER_CONFIG_H
#define HTTP_SERVER_CONFIG_H

#include <iostream>
#include <fstream>

class HTTPConfig {
public:
    HTTPConfig();

    ~HTTPConfig() = default;

    void parseConfig();

public:
    std::string getDocumentRoot();

    size_t getCPULimit();

    size_t getThreadLimit();

    size_t getPort();

private:
    std::string cfg_path;
    std::string document_root;
    size_t cpu_limit;
    size_t thread_limit;
    size_t port;
};

#endif //HTTP_SERVER_CONFIG_H
