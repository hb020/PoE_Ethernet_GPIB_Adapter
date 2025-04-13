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

void BasicWebServer::loop(int nrConnections) {
    // simple TCP server based on Ethernet and 'accept()', meaning I must handle the lifecycle of the client 
    // Right now it only handles 1 client at a time.
    // It is not blocking for input, but blocks for output
    if (client && !client.connected()) {
        client.stop();
    }
    if (!client) {
        EthernetClient newClient = server.accept();
        if (newClient) {
            client = newClient;
            currentLineIsBlank = true;
        }
    }
    if (client) {
      // an http request ends with a blank line
        while (client.connected() && client.available()) {
          char c = client.read();
          // if you've gotten to the end of the line (received a newline
          // character) and the line is blank, the http request has ended,
          // so you can send a reply
          if (c == '\n' && currentLineIsBlank) {
            sendResponse(nrConnections);
            delay(10); // yield to send reply
            client.stop();
            break;
          }
          if (c == '\n') {
            // you're starting a new line
            currentLineIsBlank = true;
          } else if (c != '\r') {
            // you've gotten a character on the current line
            currentLineIsBlank = false;
          }
        }
    }
};

void BasicWebServer::sendResponse(int nrConnections) {
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
    client.println(F("::gpib,N::INSTR</b> for the instruments, where N is their address on the GPIB bus</p>"));
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