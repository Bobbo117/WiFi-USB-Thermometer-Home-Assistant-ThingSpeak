# WiFi-USB-Thermometer-Home-Assistant-ThingSpeak
Low cost device publishes temperature, humidity to Home Assistant and ThingSpeak

## Project Description
This project encloses a temperature/humidity sensor and microcontroller in a USB stick to create a compact device capable of using WiFi to monitor the sensor using any combination of the following options:

a. Computer via USB to Arduino IDE

b. Home Assistant via MQTT protocol

c. ThingSpeak.com

## Hardware Overview
The microcontroller and temperature/humidity sensor fit inside a USB stick enclosure to create the device.  A short cable provides access to USB power.

(image)

## Hardware Components
Microcontroller (Wemos LOLIN D1 Mini ESP8266) 

Temperature/Humidity Sensor (AHT10, BME280, DHT11, 21, 22, or SHT20)

Enclosure

USB cable or adapter

USB Power source

## Software
The USB software is written in C++ using the Arduino IDE.  You need to add the following to the code to interface with Home Assistant in ThinkSpeak:

a. WiFi SSID and password

b. Home Assistant MQTT credentials

c. ThinnkSpeak credentials

## Home Assistant Setup (optional)

## ThingSpeak Setup (Optional)

## Procedure

a. Solder connections betwwen the usb8266 and the temperature/humidity sensor but not the rst - D0 link

b. Apply the applicable WIFI, MQTT, and ThinkSpeak credentials and adjust the compile time parameters accordingly

c. Compile and upload the software

d. Open the serial monitor

e. Verify the serial monitor list looks like the one below, based on selections you made

f. Verify the data made it to Home Assistant and ThingSpeak as expected

g. Note that the device will only restart if the reset button is pushed; it will not wake up on its own yet

## Final Steps

When everything is working as it should, solder a link to D0 and RST on the ESP8266.  This enables the device to wake up repeatedly.

Let it run several wakeup cycles connected to your serial monitor.  Verify all systems are as expected.

Enclose it in the enclosre and plug it in somewhere.

