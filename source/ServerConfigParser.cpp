#include <thread>
#include <fstream>
#include <sstream>
#include "ServerConfigParser.h"

unsigned int ServerConfigParser::cpu_limit_ = std::thread::hardware_concurrency();
std::string ServerConfigParser::document_root_ = "/var/www/html";
unsigned int ServerConfigParser::port_ = 80;

ServerConfig ServerConfigParser::parse(const std::string &path) {
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
        } else if (key == "document_root") {
            document_root_ = value;
        }
    }

    config_file.close();

    return ServerConfig{cpu_limit_, document_root_, port_};
}

void ServerConfigParser::setPort(unsigned int port) {
    port_ = port;
}
