#include <iostream>
#include <ServerConfig.h>
#include <Server.h>

int main() {
    ServerConfig config;
    config.parse("../httpd.conf");
    std::cout << "document_root = " << config.getDocumentRoot() << std::endl;

    try {
        auto server = std::make_unique<Server>(&config);
        server->run();

        //Server server(&config);
        //server.run();
    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
