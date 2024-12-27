
# PoE Ethernet GPIB Adapter

## Design Notes

![Adapter Assembled](../Img/adapter_open.png)

---

## Hardware

While there are a few open-source GPIB controller projects, most of them rely on USB, Raspberry Pi, or WiFi/Bluetooth connectivity. These solutions do not meet my requirements. Using over 10 adapters with USB would result in a serial port management nightmare and require a service running on a local computer to forward all ports over the network for access from any computer. 

WiFi could resolve this but would introduce multiple devices in close proximity to sensitive test setups, which I find suboptimal. One PoE-powered GPIB project based on the RP2040 was mentioned on the EEVblog forum but was never published and seemed to have stalled. Therefore, I designed my own adapter while ensuring compatibility with one of the largest open-source adapter, the AR488.

Based on these requirements, the design was divided into the following components:

- PoE Power Supply
- Microcontroller Interface
- Network Interface
- GPIB Interface
- USB Interface

---

### PoE Power Supply

Several PoE modules provide 5V output, simplifying integration. However, achieving low noise and a small form factor made off-the-shelf solutions challenging without extensive pre-testing for EMI compliance. To address this, I divided the power supply into two parts: a PoE controller and a standalone isolated power supply.

- **PoE Controller:** I selected the Texas Instruments TPS2375-1 for its small size, affordability, built-in inrush limit, integrated MOSFET, and 802.3af compliance. The device was configured for Class 0 classification with standard input capacitance and transient protection.
- **Isolated 5V Power Supply:** The Texas Instruments LM5180 flyback controller was chosen for its integrated switch and primary-side regulation, offering a simple implementation. Although flyback converters are not ideal for low EMI, space constraints left no alternative without significantly increasing complexity. To minimize EMI:
  - A powerful common-mode filter was added on the input side.
  - Planar magnetics were used to reduce current loop size.
  - A cancellation winding was added to the transformer to mitigate common-mode voltage issues.
  - A strong chassis connection to the GPIB connector ensured a low-impedance return path for common-mode voltage.

---

### Microcontroller Interface

The Microchip ATMEGA4809 was chosen for its affordability and familiarity. While a more powerful 32-bit processor could have been considered, simplicity and the 8-bit architecture's alignment with the GPIB bus made this an appealing choice.

- **Unique MAC Address:** A Microchip I2C Unique ID EEPROM was included to provide a globally unique MAC address for the Ethernet interface. It can also store user-configurable settings.
- **Additional Features:** An RGB LED and reset switch were added for status indication and recovery in case of GPIB bus or adapter failure. 
- **Expansion:** A standard UPDI pinout for Atmel ICE programmers was implemented. Optional resistors connect I2C and UART2 to unused pins for potential future use with sensors or a WiFi UART interface (default resistors are not mounted).

---

### Network Interface

The Wiznet W5500 was selected as the network controller for its available drivers and compatibility with the AR488 project. To simplify the design:
- A magnetics RJ-45 magjack was used.
- Series termination was added to all applicable I/O lines for proper signal integrity and EMI reduction.
- A Texas Instruments SN74LV1T125DCK tri-state buffer was used for level translation (5V ↔ 3.3V ↔ 5V).
- A simple LDO provided the 3.3V supply, with a watchdog on the reset line.
- The crystal was selected per the datasheet to ensure Ethernet timing compliance.

---

### GPIB Interface

The GPIB interface directly connects to the microcontroller without driver ICs, using a resistor network on each I/O line and ESD protection to ground. This design:
- Does not handle constant overvoltage but protects against most ESD events.
- Requires the device to be powered on to avoid loading the GPIB bus, deviating slightly from full IEEE-488 compliance. This compromise reduces cost and space while meeting the 99% use case of the adapter being the controller.

---

### USB Interface

USB-to-UART controllers from reputable distributors proved expensive. Instead, I selected the CH340X USB-UART controller for its affordability and proven reliability in DIY and commercial projects. Notable features:
- A USB-C connector was used and configured for 500mA power.
- Protection includes a PTC fuse, ferrite bead, transient diode, and ESD filters on the data pins.
- The only drawback is sourcing the CH340X from Asian vendors, but reputable distributors like LCSC ensure reliability.

---

## Software

The current software is a work-in-progress, simplifed fork of the AR488 project. This fork was chosen because it uses the Prologix syntax, compatible with my existing scripts. I also appreciate the Prolgix's simplicity: the Ethernet adapter operates as a basic socket. Commands without a `++` prefix are sent directly to the bus, while configuration is handled with `++` commands (e.g., `++addr` queries the address, and `++addr 22` sets the interface address to 22).

Initially, I intended to fully integrate the network stack and console interface into the AR488 project. However, given the project's complexity and my inability to test all functionalities, I opted to streamline this fork exclusively for this adapter. In the future, I plan to move away from Arduino to enhance performance.

Currently, the software includes a barebones implementation of the network stack and I2C support, with only DHCP enabled. Switching to USB-C UART instead of Ethernet requires modifying the configuration file and recompiling. Updates are planned to address these limitations.

Despite its barebones hack approach, the code has proven extremely reliable, running for months without issues. All my Prologix-based scripts work seamlessly.

Many updates on the SW is expected in the future. 

Contributions to the software are highly welcome!

---

## Mechanics

A simple split housing was designed in FreeCAD for 3D printing. The snap-action case simplifies assembly and includes optional holes for securing the PCB to the bottom half. These holes were added as a precaution in case EMI reduction required metallization or conductive 3D printing. Although these measures were unnecessary, securing the PCB prevents rattling and ensures a snug fit. There is also two holes on the back of the case for optional M3.5 screws to secure the adapter to the instrument.

The case design fitment heavily depends on the 3D printer and filament. I printed the cases on a Prusa MK3.5 using Bambu Labs PLA-CF filament, which provides a superb finish.

---

