# ESP-32-IOT-Scale
An IOT gas scale implemented with ESP-32 microcontroller, weight sensor and amplifier. it also features a front-end website to display graphically the level of gas in the cylinder
The system is fully configurable with any gas cylinder and weight sensor.
The weight sensor is first caliberated with the HX711 ADC and amplifier, this is done by running the caliberation.ino file
Then the configuration file is run on the esp-32 to create a schema for storing the full and empty weight using Preference library.
The system uploads the current weight and the percentage weight of the cylinder every 5 minutes
