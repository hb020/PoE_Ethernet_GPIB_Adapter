#pragma once

#include <Ethernet.h>
#include "config.h"

#define MAX_WEB_CLIENTS 1

class BasicWebServer {
public:
    BasicWebServer();
    void begin(bool debug = false);
    void loop(int nrConnections);

private:
    bool debug;
    int nr_connections(void);
    bool have_free_connections(void);
    void sendResponseErr(EthernetClient &client);
    void sendResponseOK(EthernetClient &client, int nrConnections);
    EthernetServer server = EthernetServer(80);
    EthernetClient clients[MAX_WEB_CLIENTS];
    bool currentLineIsBlank[MAX_WEB_CLIENTS];
    int charsRead[MAX_WEB_CLIENTS];
    uint8_t startreq[MAX_WEB_CLIENTS][6];
};
