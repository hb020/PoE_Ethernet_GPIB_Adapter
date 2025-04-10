#include "vxi_server.h"
#include "rpc_enums.h"
#include "rpc_packets.h"


VXI_Server::VXI_Server(SCPI_handler_interface &scpi_handler)
    : scpi_handler(scpi_handler)
{
    tcp_server = NULL;
}

VXI_Server::~VXI_Server()
{
}

bool VXI_Server::available() {
    if (client) return !client.connected();
    return true;
  }

uint32_t VXI_Server::allocate()
{
    uint32_t port = 0;

    if (available()) {
        port = vxi_port; // This can be a cyclic counter, not a simple integer
    }
    return port;
}

void VXI_Server::begin(uint32_t port, bool debug)
{
    vxi_port = port;
    this->debug = debug;

    if (tcp_server) {
        delete tcp_server;
        tcp_server = NULL;
    }

    tcp_server = new EthernetServer(vxi_port);
    if (!tcp_server) {
        if (debug) {
            debugPort.print(F("ERROR: Failed to create TCP server on port "));
            debugPort.printf("%u\n", (uint32_t)vxi_port);
        }
        return;
    }

    if (debug) {
        debugPort.print(F("VXI server listening on port "));
        debugPort.printf("%u\n", (uint32_t)vxi_port);
    }
    tcp_server->begin();
}


void VXI_Server::loop()
{
    if (client && !client.connected()) {
        client.stop();
        if (debug) {
            debugPort.print(F("Force Closing VXI connection on port "));
            debugPort.printf("%u\n", (uint32_t)vxi_port);
        }        
    }
    if (client) // if a connection has been established on port
    {
        bool bClose = false;
        int len = get_vxi_packet(client);

        if (len > 0) {
            bClose = handle_packet();
        }

        if (bClose) {
            if (debug) {
                debugPort.print(F("Closing VXI connection on port "));
                debugPort.printf("%u\n", (uint32_t)vxi_port);
            }
            /*  this method will stop the client and the tcp_server, then potentially rotate
                to the next port (within the specified range) and restart the
                tcp_server to listen on that port.  */
            client.stop();
        }
    } else // i.e., if ! client 
    {
        client = tcp_server->accept(); // see if a client is available (data has been sent on port)

        if (client) {
            if (debug) {
                debugPort.print(F("\nVXI connection established on port "));
                debugPort.printf("%u\n", (uint32_t)vxi_port);
            }
        }
    }
}

bool VXI_Server::handle_packet()
{
    bool bClose = false;
    uint32_t rc = rpc::SUCCESS;

    if (vxi_request->program != rpc::VXI_11_CORE) {
        rc = rpc::PROG_UNAVAIL;

        if (debug) {
            debugPort.print(F("ERROR: Invalid program (expected VXI_11_CORE = 0x607AF; received 0x"));
            debugPort.printf("%08x)\n", (uint32_t)(vxi_request->program));
        }

    } else
        switch (vxi_request->procedure) {
        case rpc::VXI_11_CREATE_LINK:
            create_link();
            break;
        case rpc::VXI_11_DEV_READ:
            read();
            break;
        case rpc::VXI_11_DEV_WRITE:
            write();
            break;
        case rpc::VXI_11_DESTROY_LINK:
            destroy_link();
            bClose = true;
            break;
        default:
            if (debug) {
                debugPort.print(F("Invalid VXI-11 procedure (received "));
                debugPort.printf("%u)\n", (uint32_t)(vxi_request->procedure));
            }
            rc = rpc::PROC_UNAVAIL;
            break;
        }

    /*  Response messages will be sent by the various routines above
        when the program and procedure are recognized (and therefore
        rc == rpc::SUCCESS). We only need to send a response here
        if rc != rpc::SUCCESS.  */

    if (rc != rpc::SUCCESS) {
        vxi_response->rpc_status = rc;
        send_vxi_packet(client, sizeof(rpc_response_packet));
    }

    /*  signal to caller whether the connection should be close (i.e., DESTROY_LINK)  */

    return bClose;
}

void VXI_Server::create_link()
{
    /*  The data field in a link request should contain a string
        with the name of the requesting device. It may already
        be null-terminated, but just in case, we will put in
        the terminator.  */

    if (!scpi_handler.claim_control()) {
        create_response->rpc_status = rpc::SUCCESS;
        create_response->error = rpc::OUT_OF_RESOURCES; // not DEVICE_LOCKED because that would require lock_timeout etc
        create_response->link_id = 0;
        create_response->abort_port = 0;
        create_response->max_receive_size = 0;
        send_vxi_packet(client, sizeof(create_response_packet));
        return;
    }

    create_request->data[create_request->data_len] = 0;
    if (debug) {
        debugPort.print(F("CREATE LINK request from \""));
        debugPort.print(create_request->data);
        debugPort.print(F("\" on port "));
        debugPort.printf("%u\n", (uint32_t)vxi_port);
    }
    /*  Generate the response  */
    create_response->rpc_status = rpc::SUCCESS;
    create_response->error = rpc::NO_ERROR;
    create_response->link_id = 0;
    create_response->abort_port = 0;
    create_response->max_receive_size = VXI_READ_SIZE - 4;
    send_vxi_packet(client, sizeof(create_response_packet));
}

void VXI_Server::destroy_link()
{
    if (debug) {
        debugPort.print(F("DESTROY LINK on port "));
        debugPort.printf("%u\n", (uint32_t)vxi_port);
    }
    destroy_response->rpc_status = rpc::SUCCESS;
    destroy_response->error = rpc::NO_ERROR;
    send_vxi_packet(client, sizeof(destroy_response_packet));
    scpi_handler.release_control();
}

void VXI_Server::read()
{
    // This is where we read from the device
    char outbuffer[256];
    size_t len = 0;
    bool rv = scpi_handler.read(outbuffer, &len, sizeof(outbuffer));

    // FIXME handle error codes, maybe even pick up errors from the SCPI Parser

    if (debug) {
        debugPort.print(F("READ DATA on port "));
        debugPort.printf("%u", (uint32_t)vxi_port);
        debugPort.print(F("; data = "));
        printBuf(outbuffer, (int)len);
    }
    read_response->rpc_status = rpc::SUCCESS;
    read_response->error = rpc::NO_ERROR;
    read_response->reason = rpc::END;
    read_response->data_len = (uint32_t)len;
    strcpy(read_response->data, outbuffer);

    send_vxi_packet(client, sizeof(read_response_packet) + len);
}

void VXI_Server::write()
{
    // This is where we write to the device
    uint32_t wlen = write_request->data_len;
    uint32_t len = wlen;
    // right trim. SCPI parser doesn't like \r\n
    while (len > 0 && isspace(write_request->data[len - 1])) {
        len--;
    }
    write_request->data[len] = 0;
    if (debug) {
        debugPort.print(F("WRITE DATA on port "));
        debugPort.printf("%u", (uint32_t)vxi_port);
        debugPort.print(F("; data = "));
        printBuf(write_request->data, (int)len);
    }
    /*  Parse and respond to the SCPI command  */
    scpi_handler.write(write_request->data, len);

    /*  Generate the response  */
    write_response->rpc_status = rpc::SUCCESS;
    write_response->error = rpc::NO_ERROR;
    write_response->size = wlen; // with the original length
    send_vxi_packet(client, sizeof(write_response_packet));
}

// const char *VXI_Server::get_visa_resource()
// {
//     static char visa_resource[40];
//     sprintf(visa_resource, "TCPIP::%s::INSTR", WiFi.localIP().toString().c_str());
//     return visa_resource;
// }

