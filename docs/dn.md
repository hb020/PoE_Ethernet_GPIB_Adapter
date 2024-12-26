
<h1>PoE Ethernet GPIB  Adapter</h1>


<h2>Design note</h2>


![Adapter_assebled](../Img/adapter_open.png)


<p>The <strong>PoE Ethernet GPIB Adapter</strong> is designed to interface test equiptment with and GPIB / HPIB / IEEE-488 interface</p>

<p>The primary goal of this project is to have an easy solution to connect multiple GPIB equipted instruments over ethernet without excess cabling.</p>
<p></p>

<hr />

<h2>Why?</h2>

<p>The motivation behind this project comes from one of the drawbacks of several generations of instruments using GPIB. In theory one only need one GPIB master and up to 20+ instrument can be connected together in what ever order you like with sutible GPIB cabling. It is however a couple of drawbacks with this:</p>

<ul>
<li>It requires the instrument to be fully compliant to the IEEE-488 standard, since older instrument was made before this standard was made, a lot of them is not compliant. Main problem with this is that the bus does not go Hi-Z when off, requiring certain instruments to be turned on, even if you only want to talk to another device.</li>
<li>GPIB cabling is bulky and expensive. Altougth good deals can be had at the used marked, the price of the cables are generally on the higher side.</li>
</ul>

<p>At work this has been solved elegantly by having a bunch of commerical availible Ethernet adapters, each is assigned an address and you just plug it in the back of the instrument. The drawback is the price of the adapter, retailing for about 500 USD. This is not hobby friendly, so my solution so far was to buy one adapter and stick with daisy-chaining. But due to the problems listed above and an instrument park that has grown to 20 devices requiring GPIB, this was not working out. And so the <strong>PoE Ethernet GPIB Adapter</strong> was born.
</p>

<hr />

<h2>Design criteria</h2>

<ul>
<li>Low cost, it should be cheaper than one single brand name GPIB interface cable and 1/10 of the price of brand name GPIB adapters.</li>
<li>Support Power over Ethernet to reduce required cables needed.</li>
<li>Support to be powered over USB-C should not PoE be available.</li>
<li>Support GPIB communication over either the ethernet interface or thru serial USB-C interface.</li>
<li>Use the same communication protocal as other avaialbe commercial units over telnet interace.</li>
<li>Have low radiated and conducted noise as to not interfere with the test enviroment.</li>
<li>Have an easy to print 3d-printed enclosure to keep it simple and low cost.</li>
</ul>


