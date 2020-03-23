#include "config.h"

HTTPConfig::HTTPConfig() {
    cfg_path = "../httpd.conf";
    document_root = "";
    cpu_limit = 0;
    thread_limit = 0;
    port = 80;
}

void HTTPConfig::parseConfig() {
    std::ifstream cfg_file(cfg_path);
    std::string line;

    while (!cfg_file.eof()) {
        std::getline(cfg_file, line, '\n');

        if (line.empty()) {
            break;
        }

        size_t space_position = line.find_first_of(' ');

        if (line.substr(0, space_position) == "cpu_limit") {
            cpu_limit = stoi(line.substr(space_position + 1, line.find_first_of(' ') - space_position - 1));
        }

        if (line.substr(0, space_position) == "thread_limit") {
            thread_limit = stoi(line.substr(space_position + 1, line.find_first_of(' ') - space_position - 1));
        }

        if (line.substr(0, space_position) == "document_root") {
            document_root = line.substr(space_position + 1, line.find_first_of(cfg_file.eof()) - space_position - 1);
        }
    }

}

std::string HTTPConfig::getDocumentRoot() {
    return document_root;
}

size_t HTTPConfig::getCPULimit() {
    return cpu_limit;
}

size_t HTTPConfig::getThreadLimit() {
    return thread_limit;
}

size_t HTTPConfig::getPort() {
    return port;
}
