#include "config.h"

#ifdef USE_WEBSERVER
#include <Arduino.h>
#include <Ethernet.h>
#include "web_server.h"

BasicWebServer::BasicWebServer() {
    // Constructor
}

void BasicWebServer::begin() {
    server.begin();
}

int BasicWebServer::nr_connections(void) {
    int count = 0;
    for (int i = 0; i < MAX_WEB_CLIENTS; i++) {
        if (clients[i]) {
            count++;
        }
    }
    return count;
}

bool BasicWebServer::have_free_connections(void) {
    for (int i = 0; i < MAX_WEB_CLIENTS; i++) {
        if (!clients[i]) {
            return true;
        }
    }
    return false;
}

void BasicWebServer::loop(int nrConnections) {
    // simple TCP server based on Ethernet and 'accept()', meaning I must handle the lifecycle of the client 
    // It is not blocking for input, but blocks for output

    // close any clients that are not connected
    for (int i = 0; i < MAX_WEB_CLIENTS; i++) {
        if (clients[i] && !clients[i].connected()) {
            clients[i].stop();            
        }
    }

    // check if a new client is available
    EthernetClient newClient = server.accept();
    if (newClient) {
        bool found = false;
        for (int i = 0; i < MAX_WEB_CLIENTS; i++) {
            if (!clients[i]) {
                clients[i] = newClient;
                found = true;
                currentLineIsBlank[i] = true; // init parser
                break;
            }
        }
        if (!found) {
            newClient.stop();
        }
    }

    // handle any incoming data
    for (int i = 0; i < MAX_WEB_CLIENTS; i++) {
        // an http request ends with a blank line
        while (clients[i].connected() && clients[i].available()) {
            char c = clients[i].read();
            // if you've gotten to the end of the line (received a newline
            // character) and the line is blank, the http request has ended,
            // so you can send a reply
            if (c == '\n' && currentLineIsBlank[i]) {
              sendResponse(clients[i], nrConnections);
              delay(10); // yield to send reply
              clients[i].stop();
              break;
            }
            if (c == '\n') {
              // you're starting a new line
              currentLineIsBlank[i] = true;
            } else if (c != '\r') {
              // you've gotten a character on the current line
              currentLineIsBlank[i] = false;
            }
          }    
    }    
};

void BasicWebServer::sendResponse(EthernetClient &client, int nrConnections) {
    // send a standard http response header
    client.println(F("HTTP/1.1 200 OK\nContent-Type: text/html\nConnection: close\nRefresh: 5\n"));  // refresh the page automatically every 5 sec
    client.println(F("<!DOCTYPE HTML>\n<html><head><title>Ethernet2GPIB</title><style>body { font-family: Arial, sans-serif; }</style></head><body>"));
    client.print(F("<h1>" DEVICE_NAME "</h1>"));
    client.print(F("<p>Number of connections: "));
    client.print(nrConnections);
    client.println(F("</p>"));
#ifdef INTERFACE_VXI11
    client.println(F("<h2>VXI-11 Ethernet Server</h2>"));
    client.print(F("<p>VISA connection strings:</p><p><b>TCPIP::"));
    client.print(Ethernet.localIP());
    client.print(F("::INSTR</b> for the controller, and</p><p><b>TCPIP::"));
    client.print(Ethernet.localIP());
    client.println(F("::gpib,<i>N</i>::INSTR</b> for the instruments, where <i>N</i> is their address on the GPIB bus</p>"));
#endif
#ifdef INTERFACE_PROLOGIX
    client.println(F("<h2>Prologix GPIB Ethernet Server</h2>"));
    client.print(F("<p>IP Address: "));
    client.print(Ethernet.localIP());
    client.println(F("</p>"));
#endif
    client.println("</body></html>");
    client.println();
}
#endif