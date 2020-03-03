#include <iostream>
#include <ServerConfigParser.h>
#include <Server.h>

int main() {
    auto server_config = ServerConfigParser::parse("../httpd.conf");
    std::cout << "document_root = " << server_config.document_root << std::endl <<
              "cpu_limit = " << server_config.cpu_limit << std::endl;

    try {
        Server s(server_config);
        s.run();
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
