# WiFi-USB-Thermometer-Home-Assistant-ThingSpeak
Low cost device encloses a temperature/humidity sensor in a USB stick capable of publishing to Home Assistant and ThingSpeak

## Project Description
This project encloses a temperature/humidity sensor and microcontroller in a USB stick to create a compact device capable of using WiFi to monitor the sensor using any combination of the following options:

a. Computer via USB to Arduino IDE

b. Home Assistant via MQTT protocol

c. ThingSpeak.com

## Hardware Overview
The microcontroller and temperature/humidity sensor fit inside a USB stick enclosure to create the device.  A short cable provides access to USB power.  Holes at each end of the enclosure allow heat to escape.

The components are available from multiple sources.  Best prices are from AliExpress, but the shipping costs are a killer.  Amazon has everything, but in multiple quantities.  Pick your poison.  Get on the Google.

![image](https://github.com/Bobbo117/WiFi-USB-Thermometer-Home-Assistant-ThingSpeak/blob/main/Images/Exploded_View.jpg)

## Hardware Components


### Microcontroller (Wemos LOLIN D1 Mini ESP8266) 

![image](https://github.com/user-attachments/assets/deb7757e-022d-40d1-9cd0-f9b4820fb36a)

![image](https://github.com/user-attachments/assets/30f0dc68-d9a5-49dc-9d5f-92cf92265db8)


## Temperature/Humidity Sensor (AHT10, BME280, or SHT20)

![image](https://github.com/Bobbo117/WiFi-USB-Thermometer-Home-Assistant-ThingSpeak/blob/main/Images/AHT10.jpg)


![image](https://github.com/Bobbo117/WiFi-USB-Thermometer-Home-Assistant-ThingSpeak/blob/main/Images/BME280.jpg)


![image](https://github.com/Bobbo117/WiFi-USB-Thermometer-Home-Assistant-ThingSpeak/blob/main/Images/SHT20.jpg)


### Enclosure

### USB cable or adapter

### USB Power source - several are available.  I prefer the Aiken because the usb socket is parallel to the wall:


![image](https://github.com/Bobbo117/WiFi-USB-Thermometer-Home-Assistant-ThingSpeak/blob/main/Images/power_adapters.jpg)


# Hardware Connections

5 volt power is supplied to the ESP8266 via USB connection.

Temperature/Humidity Sensor pins are connected to the esp8266 as follows:

1. VCC to esp8266 3.3v
2. GND to esp8266 GND
3. SCL to esp8266 D1
4. SDA to esp8266 D2 

## Software
The USB software is written in C++ using the Arduino IDE.  You need to add the following to the code to interface with Home Assistant in ThingSpeak:

a. WiFi SSID and password

b. Home Assistant MQTT credentials

c. ThinngSpeak credentials

## Home Assistant Setup (optional)

As an option, this project publishes to Home Assistant (HA) via topic "device1/temperature" and "device1/humidity".

To implement it in HA, open the file editor to mqtt.yaml, and enter the following under the sensor section:


![image](https://github.com/Bobbo117/WiFi-USB-Thermometer-Home-Assistant-ThingSpeak/blob/main/Images/mqtt_yaml.jpg)


Save the file and Navigate to the Developer Tools section. Reload the "All YAML Configuration.

Open the dashboard User Interface (Lovelace in my case) to create a gauge card for temperature and another for humidity:


![image](https://github.com/Bobbo117/WiFi-USB-Thermometer-Home-Assistant-ThingSpeak/blob/main/Images/HA_guage_card.jpg)


Create a History Graph Card with the following parameters (or not!):


![image](https://github.com/Bobbo117/WiFi-USB-Thermometer-Home-Assistant-ThingSpeak/blob/main/Images/HA_history_card.jpg)


Select DONE to display the result:


![image](https://github.com/Bobbo117/WiFi-USB-Thermometer-Home-Assistant-ThingSpeak/blob/main/Images/HA.jpg)


## ThingSpeak Setup (Optional)

As an option, this project publishes to ThingSpeak.


### To implement it in ThingSpeak, configure a new channnel named Device1 (or some other name) and save it.


![image](https://github.com/Bobbo117/WiFi-USB-Thermometer-Home-Assistant-ThingSpeak/blob/main/Images/ThingSpeak_channel_settings.jpg)


## You will need to record the Channel ID and the write API for the channel.  These are the credentials you need to publish data.



### Create a temperature guage widget

![image](https://github.com/Bobbo117/WiFi-USB-Thermometer-Home-Assistant-ThingSpeak/blob/main/Images/ThingSpeak_temperature_guage_config.jpg)

### Create a humidity guage widget


![image](https://github.com/Bobbo117/WiFi-USB-Thermometer-Home-Assistant-ThingSpeak/blob/main/Images/ThingSpeak_humidity_guage_config.jpg)


### The following Screens will be available when ThingSpeak receives data:


![image](https://github.com/Bobbo117/WiFi-USB-Thermometer-Home-Assistant-ThingSpeak/blob/main/Images/ThingSpeak_stats.jpg)


![image](https://github.com/Bobbo117/WiFi-USB-Thermometer-Home-Assistant-ThingSpeak/blob/main/Images/ThingSpeak_guages.jpg)


## Build Procedure

a. Solder connections betwwen the esp8266 and the temperature/humidity sensor but not the rst - D0 link

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

## Serial Monitor Printout

The serial monitor list should be similar to the one below, based on selections you made. 

![image](https://github.com/Bobbo117/WiFi-USB-Thermometer-Home-Assistant-ThingSpeak/blob/main/Images/printout.jpg)

Notice that there is a temperature variable and a tem variable shown.  The temperature displays the most recent valid temperature.
If the most recent temperature was invalid, then the tem value displays "--"; otherwise tem = temperature.  The temperature variable is published.
The same is true of the humidity and hum variables.  This approach keeps the published history graphs in a reasonable range while at the same time providing a debug/verification mechanism.
