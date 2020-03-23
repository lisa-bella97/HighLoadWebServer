#include "server/server.h"

int main() {
    HTTPConfig cfg_file;
    cfg_file.parseConfig();
    auto server = std::make_unique<HTTPServer>(&cfg_file);
    server->serverStart();
}
