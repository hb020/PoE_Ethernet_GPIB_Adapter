
# PoE Ethernet GPIB Adapter

GOAL OF THIS FORK: add pyvisa compatibility, be it RAW or VXI-11.2

STATUS OF THIS FORK: WIP

This code can produce either a VXI-11.2 device, or a Prologix device (the ROM is not big enough for both at the same time, although the code is compatible with cohabitation)

* Switching between VXI-11 and prologix is done at compile time. If [config.h](/SW/src/config.h) `#define INTERFACE_PROLOGIX` or compiler option `-DINTERFACE_PROLOGIX` is used, the firmware produced is for prologix.
* VXI-11.2 server works, is robust and fast. (with limitations: no support for device sub-addresses (is WIP in AR488), and no interrupts). Supports up to 30 client-instrument combinations. Should be enough. Requires no config nor eeprom, and is discoverable over the network.
* Prologix server works as before.
* the LED now indicates different states: blue for waiting for DHCP, red for error in DHCP, green flashing for idle, green/blue flashing for busy
* prepared for a serial console, only logs to it right now.
* mdns is not possible. tried multiple libraries, they do not work for this type of AVR
* loads of RAM still available (4k+)
* fully integrated with platformio (but can be compiled also outside of platformio)
* RAW server is abandoned, as it would need many more resources.
* It is based upon the latest AR488 (v0.53.03). Documented the adaptations in the document [Relation_to_AR488](/SW/src/Relation_to_AR488.md).
* Added a basic web server with stats and info (just because I can)

TODO:

* clean up code
* document
* add more tests

TODO that I cannot do, would need your help with that:
* test with other instruments. Right now I do reads that are equivalent to "++read eoi". Might not be good for all instruments.


## PoE-powered GPIB Adapter with Ethernet and USB-C Support

<a href="Img/adapter_connected.png" target="_blank">
    <img src="Img/adapter_connected.png" alt="Adapter Assembled" width="800">
</a>

The **PoE Ethernet GPIB Adapter** is designed to interface test equipment with GPIB / HPIB / IEEE-488 interfaces.

The primary goal of this project is to provide an easy solution to connect multiple GPIB-equipped instruments over Ethernet without excessive cabling.

---

## Why?

The motivation behind this project comes from the challenges of using several generations of instruments with GPIB interfaces. In theory, only one GPIB master is needed, and up to 20+ instruments can be connected in any order using suitable GPIB cabling. However, there are drawbacks:

- Instruments must be fully compliant with the IEEE-488 standard. Many older instruments, predating the standard, do not comply. The main issue is that the bus does not go Hi-Z when off, requiring certain instruments to be powered on even if you're only using another device.
- GPIB cabling is bulky and expensive. While used options can sometimes be found, the overall cost remains high.

At work, this has been solved elegantly using commercial Ethernet adapters, each assigned an address and connected directly to the instrument. However, these adapters are priced around $500 USD—far from hobbyist-friendly. My initial solution was to buy one adapter and rely on daisy-chaining, but as my instrument collection grew to 20 devices requiring GPIB, this approach was no longer practical. Hence, the **PoE Ethernet GPIB Adapter** was born.

---

## Design Criteria

1. Lower cost: Total BOM should be less than 50 USD.
2. Support Power over Ethernet (PoE) to minimize cable clutter.
3. Include USB-C power as an alternative if PoE is unavailable.
4. Enable GPIB communication over both Ethernet and USB-C interfaces.
5. Use the same communication protocol as existing commercial units (via Telnet).
6. Minimize radiated and conducted noise to avoid interference in test environments.
7. Include a simple, easy-to-print 3D-printed enclosure to keep costs low.

---

## Results

All design goals have been met. The current unit price is approximately $45 USD when ordering parts for 20 units. Prices increase for smaller batch sizes.

In my home lab, I assign each device a static IP address based on its MAC address and run a local DNS server to provide easy-to-remember domain names, making the adapter simple and intuitive to use.

Project's documentation:

- [Design Documentation](docs/dn.md)
- [Design Test Documentation](docs/dt.md)

---

## Project files

Latest version is avaiable under [Releases](https://github.com/Kofen/PoE_Ethernet_GPIB_Adapter/releases)

And all the source code for all the parts should be in their respective folders in the repro. If anything is missing, let me know!

## License

This project is licensed under the GPL V3. See the [LICENSE](LICENSE) file for details.

## Acknowledgements

- A huge thanks to the [AR488 project](https://github.com/Twilight-Logic/AR488), run by [Twilight-Logic](https://github.com/Twilight-Logic) and its community contributors. The current software is a fork of AR488.

