# prologix vs RAW vs VXI-11.2 vs hislip

## Needs in number of network connections

..provided we need compatibility with 1 pyvisa program only.

Suppose N is the number of instruments to be represented:

* prologix: 1: TCP server
* RAW: N or N+1 (if you also want the instrument server): all TCP servers
* VXI-11.2: N+2 or N+3 (if you also want the instrument server): 1 UDP portmapper, 1 TCP portmapper and the rest RPC servers
* hislip: not investigated, requires a LOT

## prologix

not suppored by pyvisa

## RAW

Example of connection string:

TCPIP::192.168.1.105::[port number]::SOCKET

Requires

## VXI-11.2

Example of connection string:

* TCPIP::192.168.1.105::gpib,5::INSTR   (gpib or hpib or gpibN or hpibN): VXI-11.2
* TCPIP::192.168.1.105::inst::INSTR     (instN): 'normal' vxi-11

gpib0 or inst0: default or only, hpibN or instN: others

Requires 3 servers:

* port mappers on UDP and TCP that point to the VXI-11 RPC server
* the VXI-11 RPC server. VXI-11 consists of 3 separate RPC programs with the numeric identifiers: 0x0607AF (DEVICE_CORE), 0x0607B0 (DEVICE_ASYNC) and 0x0607B1 (DEVICE_INTR).
* you may also add mDNS

For compatibility with basic pyvisa use, you only need DEVICE_CORE, but the guidelines say you should also support the others.

While in theory one could re-use an existing VXI-11 RPC server socket, thereby only requiring 3 socket connections, 
pyvisa requires 1 client socket per device. It doesn't just use create_link on the same socket with a different device name.

Other inspiration: See https://github.com/coburnw/python-vxi11-server
Uses TCPServer and threading.

Maybe go multiplexing? Because that is what arduino will do. TODO: will it?

If we can make it work, the device will NOT support:

* secondary instrument addresses
* more than 1 client per instrument
* async VXI-11 operations
* instrument locking via VXI-11
* VXI-11 interrupts
* the VXI-11 abort channel
