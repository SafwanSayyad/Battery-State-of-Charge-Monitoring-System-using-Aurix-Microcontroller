# Battery SoC Monitoring (Infineon Hackathon)
### **Team: Batmon 17 | Board: AURIX TC334 Lite Kit**

## What is this?
We built this project during the **Infineon Hackathon**. The goal was to create a system that tracks the "State of Charge" (SoC) of a battery using a high-end automotive microcontroller.

## The "Real World" Challenge
When we started, we were given a code base that was **non-functional**. A big part of our success was digging into the source files and debugging hardware-level issues that were stopping the system from working.

### **What we fixed:**
* **Found the "Hidden" ADC Error:** The code wouldn't compile because the ADC channel declaration was accidentally commented out in the configuration files. We tracked it down and enabled it.
* **Corrected Sensor Routing:** The system was originally trying to read the onboard potentiometer (Channel 0). We fixed the logic to read the actual battery voltage from the **AN1** pin on the Lite Kit. 
* **Telemetry Setup:** We set up a UART link at 115200 baud so we could see the live battery data on a serial monitor while debugging.

## How it Works
We used the **Coulomb Counting** method to track battery health. It basically measures the current flowing in and out over time to calculate how much charge is left in the battery.
