#include <iostream>
#include <ServerConfigParser.h>

int main() {
    auto server_config = ServerConfigParser::parse("../httpd.conf");
    std::cout << "document_root = " << server_config.document_root << std::endl <<
              "cpu_limit = " << server_config.cpu_limit << std::endl;

    return 0;
}
