# COON PUNCHER, ESP32-C3 #

This repository contains the source software that shall run on the ESP32-C3 SOM as part of the Coon Puncher (Electronic Trap Actuator). <br><br>

The SOM is responsible for main processing tasks, reading from the ultrasonic sensor, and sending emails to the user over WiFi.  There is not a co-processor running in tandem with the SOM.  

### ARCHITECTURE ###
The ESP32-C3 SOM can be bootloaded over the USB-to-serial interface.  In addition to bootloading, said USB-to-serial interface will serve up a CLI that will allow access to basic debugging routines.  The entire application was written and will be maintained in the Arduino IDE. <br>

### TOOL SUITE ###

The source file(s) are to be compiled using Arduino IDE v1.8.8, or newer.   <br>

### COMPILING ###
The Coon Puncher (Electronic Trap Actuator) runs on an `ESP32-C3-MINI-1-N4` WiFi SOM.  For proper compilation, the target board shall be *ESP32C3 Dev Module*.  <br><br>

It is necessary to install the ESP Mail Client library in order for proper compilation.  The following instructions can be followed to install the library.  
Go to `Sketch > Include Library > Manage Libraries` and search for **ESP Mail Client** (not ESP**32**). Install the ESP Mail Client library which was written by **Mobizt**.  The reader shall take caution not to confuse **ESP** Mail Client with ESP**32** Mail Client.  

### VERSIONS ###
* v1.0.0 -- This version needs to be tested, but this is a release candidate.  

* v1.1.0 -- Measuring distance every 100ms opposed to every second (10× more frequently). Logging has been disabled to improve algorithm efficiency.  

* v1.2.0 -- The algorithm was modified such that calibration happens continuously.  If a new reading is shorter by 5% as compared to the previous reading, the trap will be triggered.  The first state is now to reset the trap instead of measuring distance. 

* v1.3.0 -- Filtering has been added.  The filtering allows the trap to be more resilient to premature trips from false-positives.  Also, the distance reading is an average of multiple samples.  Lastly, the trap will automatically "recalibrate" every hour.  