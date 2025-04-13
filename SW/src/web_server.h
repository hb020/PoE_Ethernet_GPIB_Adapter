#pragma once

#include <Ethernet.h>
#include "config.h"

class BasicWebServer {
public:
    BasicWebServer();
    void begin();
    void loop(int nrConnections);

private:
    void sendResponse(int nrConnections);
    EthernetServer server = EthernetServer(80);
    EthernetClient client;
    bool currentLineIsBlank = false;
};
