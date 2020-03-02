#include <thread>
#include <fstream>
#include <sstream>
#include "ServerConfigParser.h"

int ServerConfigParser::cpu_limit = std::thread::hardware_concurrency();
std::string ServerConfigParser::document_root = "/var/www/html";

config ServerConfigParser::parse(const std::string &path) {
    std::ifstream config_file(path);
    if (!config_file.is_open()) {
        throw std::runtime_error("cannot open config file");
    }

    std::string line;
    while (std::getline(config_file, line)) {
        std::istringstream line_stream(line);
        std::string arg_name, arg_val;
        line_stream >> arg_name >> arg_val;

        if (arg_name == "cpu_limit") {
            cpu_limit = std::atoi(arg_val.c_str());
        } else if (arg_name == "document_root") {
            document_root = arg_val;
        }
    }

    config_file.close();

    return config{cpu_limit, document_root};
}
