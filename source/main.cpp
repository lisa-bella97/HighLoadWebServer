#include <iostream>
#include <ServerConfigParser.h>
#include <Server.h>

int main() {
    auto server_config = ServerConfigParser::parse("../httpd.conf");
    std::cout << "document_root = " << server_config.document_root << std::endl <<
              "cpu_limit = " << server_config.cpu_limit << std::endl;

    try
    {
        boost::asio::io_service io_service;

        server s(io_service, 80);

        io_service.run();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
