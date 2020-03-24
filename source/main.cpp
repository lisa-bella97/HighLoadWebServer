#include <iostream>
#include <ServerConfig.h>
#include <Server.h>

int main() {
    ServerConfig config;
    config.parse("/etc/httpd.conf");

    try {
        Server server(&config);
        server.run();
    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
