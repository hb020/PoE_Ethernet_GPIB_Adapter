
# PoE Ethernet GPIB Adapter

## Design Tests

<a href="../Img/adapter_open.png" target="_blank">
    <img src="../Img/adapter_open.png" alt="Adapter Assembled" width="800">
</a>

---

## Scope of Testing

Each of the main blocks of the adapter was fully design-tested. The prototype (shown in the header image) was built in stages to make testing each module manageable as it was brought up.

The adapter was built and tested in this order:

- PoE Power Supply
- Microcontroller Interface
- USB Interface
- Network Interface
- GPIB Interface

**A note on details:** There are screenshots and CSV files of many measurements in this repository, but I decided against adding them to this document as they are not that interesting unless you are testing the design yourself. They're there for those who might care, in the `DT/Figs` folder. Naming should make sense for anyone familiar with the tests.

---

## Electrical Testing

### PoE Transformer

I started with a bare PCB to characterize the transformer using an HP4395A impedance analyzer in shunt-thru mode.

**Test Setup:**  
<a href="../Img/trafo_dt.png" target="_blank">
    <img src="../Img/trafo_dt.png" alt="Transformer Test Setup" width="800">
</a>

Measurements were taken for primary/secondary inductance, leakage inductance, and interwinding capacitance. Everything was calibrated, and fixture parasitics were compensated.

Two tests were run with cores providing 50nH and 100nH Al values. Eventually, the 50nH core combination was chosen (reasons detailed later). Results for this configuration:

- Interwinding capacitance: **100 pF**
- Primary inductance: **18.2 µH**
- Secondary inductance: **2.29 µH**
- Primary leakage inductance: **395 nH**

These numbers lined up well with expectations.

### PoE Flyback 5V Power Supply

With the flyback controller populated, I connected it to a lab power supply with current limiting. It came to life at around 36.5V input, outputting 5.4V, which stabilized to 5.01–4.9V depending on the load.

**Test Setup:**  
<a href="../Img/test_poe.png" target="_blank">
    <img src="../Img/test_poe.png" alt="Power Test Setup" width="800">
</a>

**Troubleshooting and Adjustments:**  
Things went sideways at 48V, with instability in the power supply. The issue was traced to an undersized air gap in the transformer core. I took a calculated risk here, knowing the available cores and space constraints. To fix this, I replaced both of the core halfs with a gapped one, reducing the Al value to 50nH. This brought the primary inductance to ~15 µH. Not ideal, as it increased magnetizing current and reduced efficiency, but it was the best trade-off given the circumstances.

- Switching node voltage was clean, with the zener snubber doing its job well. No RC snubber was needed.
- High-frequency ringing across the rectifier diode required a 330pF + 33Ω snubber, which worked effectively.

**Efficiency Plot (Clickable):**  
<a href="../Img/eff.png" target="_blank">
    <img src="../Img/eff.png" alt="Power Efficiency" width="800">
</a>

As expected, efficiency isn't great. Lower loads and higher input voltages exacerbate losses due to increased magnetizing current and switching losses. However, thermals remain under control, with no concerning hotspots under typical operating conditions. It was decided to leave the core as is until full system tests are done.

Next, stability testing was done. Since the secondary voltage is regulated by the primary side, we do not have a normal feedback network. 
To test stability, two tests were done: step response and output impedance measurement. 

First, step response was done.

**Step Response Plot (Clickable):**  
<a href="../Img/Step_response_0p5A.png" target="_blank">
    <img src="../Img/Step_response_0p5A.png" alt="Step Response" width="800">
</a>

The load was stepped from 0.1 A to 0.5 A. The result is a clean, fast step response with minimal ringing. The output voltage does drop slightly with increased load; this is due to the extra diode drop, not accounted for by the controller due to the regulating mechanism. 

Output ripple was measured to 15 mV peak to peak. Created mostly by the rest of the circuitry connected to the power supply.

As a final test, the output impedance was measured with all the parts placed on the PCB, with the adapter powered off and on. 

**5V PSU Impedance Plot (Clickable):**  
<a href="../Img/PSU_Impedance.png" target="_blank">
    <img src="../Img/PSU_Impedance.png" alt="PSU Impedance" width="800">
</a>

With the power supply off, we see a resonance formed by the output LC filter, then we turn inductive around 150 kHz. 
With the power supply turned on, those points shift, and the Q of the LC resonance is lowered due to the parallel output impedance of the power supply. 
We also lower the general output impedance and see no added peaks or resonances from the power supply itself. 

The peak from the LC filter harmonizes well with the damped half-cycle ringing observed in the step response. 

Since we have no way to inject a test signal and do not see any form of resonance created by the switching controller on the impedance plot, we have no way of estimating the phase margin, but we can safely assume it is high due to the lack of any formed resonance. 
It is concluded that the power supply is sufficiently stable. 

### Microcontroller Interface

The microcontroller and its support circuitry were populated. Programming over JTAG worked perfectly, and all LEDs blinked as expected. No issues arose.

### USB Interface

With the CH340X USB interface populated, the adapter was recognized by my computer, and the serial console worked flawlessly. The reset line behaved correctly for USB programming. Measurements revealed no ringing or anomalies on the I/O lines.

### Network Interface

The Wiznet W5500's 3.3V power supply measured 4.2V due to an ordering mistake: the AZ1117-3.3 LDO I received had a different pinout. After replacing it, the supply output 3.303V, as expected.

Once the correct LDO was in place, the W5500 connected to the network, obtained an IP address, and passed signal integrity checks.

### GPIB Interface

The final test involved connecting the adapter to a GPIB-equipped instrument and querying `*IDN?`. It returned the expected instrument ID. The adapter handled multiple instruments and longer GPIB chains without issues, with signal levels and integrity well within acceptable ranges. I set up an automated test to stress the GPIB handling by sending a lot of data back and forth between numerous instruments without any issues. 

---

## EMC Compliance Testing

**Disclaimer:** This was done at home with DIY equipment. While I've characterized all my gear and even compared some of my equipment with professional gear, these results shouldn't be treated as commercial-grade compliance tests. The goal here was to ensure the adapter wasn't creating an EMI disaster in my own lab.

### Conducted Emissions

**Test Setup:**  
<a href="../Img/emc_cond.jpg" target="_blank">
    <img src="../Img/emc_cond.jpg" alt="EMC Conducted Setup" width="800">
</a>

This worst-case setup involved elevating the DUT 2.5 cm, using a cheap 1 Meter unshielded Ethernet cable, and grounding the GPIB connector via an additional GPIB cable.

**Results (Clickable):**  
<a href="../Img/Conducted.png" target="_blank">
    <img src="../Img/Conducted.png" alt="Conducted Emissions" width="800">
</a>

The adapter met CISPR-22 Class B limits. Margins were tight at higher frequencies, but no further tweaks were deemed necessary.

### Radiated Emissions

**Test Setup:**  
<a href="../Img/tem.png" target="_blank">
    <img src="../Img/tem.png" alt="EMC Radiated Setup" width="800">
</a>

A small TEM cell was used instead of a proper far-field setup, so take these results with a grain of salt.
I have calculated the equivalent 3-meter far field, but I am still unsure how well it translates. Uncorrected as well as corrected data is therefore shown below.

**Uncorrected Results (Clickable):**  
<a href="../Img/Radiated_emissions_peak.png" target="_blank">
    <img src="../Img/Radiated_emissions_peak.png" alt="Radiated Emissions" width="800">
</a>

**Corrected Results (Clickable):**  
<a href="../Img/Radiated_emissions_peak_corrected.png" target="_blank">
    <img src="../Img/Radiated_emissions_peak_corrected.png" alt="Corrected Radiated Emissions" width="800">
</a>

The adapter showed minimal issues. Peaks around 250 MHz likely originate from the network interface.

---

## Thermal Testing

Thermal tests showed the adapter's housing raised internal temperatures by ~10°C. With the case opened, thermal imaging identified the 3.3V LDO and Ethernet interface as the hottest points (~23°C above ambient).

**Thermal Plot (Clickable):**  
<a href="../Img/Thermals.png" target="_blank">
    <img src="../Img/Thermals.png" alt="Thermal Analysis" width="800">
</a>

This suggests the adapter is safe for ambient temperatures up to ~50°C.

---

## Total power consumption

The total power consumption at 52v in was measured to 1.3 Watt. This is with network interface active, but GPIB bus idle.

## Conclusion

The device meets design goals despite a few compromises:

- The transformer core was adjusted to avoid re-sourcing or grinding cores.
- The LDO issue was fixed with a correct part replacement.

For the release version, I added optional I2C and UART2 connections to the JTAG header and improved copper pours around the Ethernet controller and LDO.

These changes aren't critical but should make future revisions easier.


