#pragma once

#include <Ethernet.h>
#include "config.h"

#define MAX_WEB_CLIENTS 10

class BasicWebServer {
public:
    BasicWebServer();
    void begin();
    void loop(int nrConnections);

private:
    int nr_connections(void);
    bool have_free_connections(void);
    void sendResponse(EthernetClient &client, int nrConnections);
    EthernetServer server = EthernetServer(80);
    EthernetClient clients[MAX_WEB_CLIENTS];
    bool currentLineIsBlank[MAX_WEB_CLIENTS];
};
