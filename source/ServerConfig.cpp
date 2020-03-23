#include <thread>
#include <fstream>
#include <sstream>
#include "ServerConfig.h"


ServerConfig::ServerConfig(unsigned int port) : port_(port),
                                                cpu_limit_(std::thread::hardware_concurrency()),
                                                thread_limit_(0) {}


void ServerConfig::parse(const std::string &path) {
    std::ifstream config_file(path);
    if (!config_file.is_open()) {
        throw std::runtime_error("cannot open config file");
    }

    std::string line;
    while (std::getline(config_file, line)) {
        std::istringstream line_stream(line);
        std::string key, value;
        line_stream >> key >> value;

        if (key == "cpu_limit") {
            cpu_limit_ = std::atoi(value.c_str());
        } else if (key == "thread_limit") {
            thread_limit_ = std::atoi(value.c_str());
        } else if (key == "document_root") {
            document_root_ = value;
        }
    }

    config_file.close();
}

std::string ServerConfig::getDocumentRoot() {
    return document_root_;
}

unsigned int ServerConfig::getPort() {
    return port_;
}
