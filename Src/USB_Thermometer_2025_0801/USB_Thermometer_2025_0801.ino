
#define VERSION "USB Thermometer v2025_0801"

/*
 * 8/1 add DS DS18B20 temperaure probe
 */
/*     IMPORTANT:
    1. Read project description at https://github.com/Bobbo117/WiFi-USB-Thermometer-Home-Assistant-ThingSpeak
       This software is designed for the LOLIN(Wemos) D1 mini esp8266.
       Copyright 2025 Bob Jessup MIT License at https://github.com/Bobbo117/WiFi-USB-Thermometer-Home-Assistant-ThingSpeak/blob/main/LICENSE

    2. To use timer wake up with ESP8266, you need to connect the RST pin to the D0 pin (GPIO16)after loading this firmware.
       Disconnect the RST pin from D0 pin while updating the firmware (this compiled software).
       
    3. This project can report data via the following: 
        a. Computer monitor via USB to Arduino IDE  
        b. Home Assistant via WIFI and MQTT if the #define WIFI_ and #define MQTT_ statements are enabled
        c. ThingSpeak if the #define WIFI_ and #define THINGSPEAK_ are enabled
*/    
 
//////////////////////////////////////////////////////////////  
//*******     Setup secrets.h file               ***********//
//                                                        //
//////////////////////////////////////////////////////////////

  #include <secrets.h>   //<--*** if you want to report to Thingspeak or MQTT
                         // Include the secrets.h file OR disable it and enable the code below  
/* 
  secrets.h file contents follow (optional):

    #define SECRET_WIFI_SSID           "your wifi ssid"
    #define SECRET_WIFI_PASSWORD       "your wifi password" 

  Home Assistant or other MQTT broker (optional):
    #define SECRET_MQTT_HOST "your mqtt host"  //12/29/2022
    #define SECRET_MQTT_PORT 1883
    #define SECRET_MQTT_USER_NAME "your mqtt user name"
    #define SECRET_MQTT_USER_PWD  "your mqtt password" 

  Thingspeak API key (optional):
    #define SECRET_THINGSPEAK_API_KEY_Device1 = "xxxxx"
    #define SECRET_AQI_THINGSPEAK_API_KEY_Device1 "xxxxx"
*/
//////////////////////////////////////////////////////////////  
//*******         Compile-time Options           ***********//
//        (Disable unwanted options with leading //)
//////////////////////////////////////////////////////////////

#define WIFI_           // enable wifi if no leading //; must enable wifi if ThingSpeak or MQTT enabled.
#define THINGSPEAK_     // enable ThingSpeak if no leading //
#define MQTT_           // enable MQTT if no leading //

//***Select ONE temperature/humidity sensor by removing the leading //
//#define AHT10_        // Adafruit AHT10 
//#define BME_          // BME280 FL
//#define DHT_          // DHT11,21,22, etc.
#define DS18B20_        // 
//#define SHT20_        // DFRobot SHT20

//***Parameter Definition values determined by user
#define deviceID 1      // Give each device a unique ID if there is more than one.
uint64_t sleepMinutes = 10;  // sleep time in minutes; max sleep time -> 71 minutes = 4260 seconds

//////////////////////////////////////////////////////////////  
//*******         Libraries & Variables          ***********//
// Use the Arduino Library Manager to install libraries for selected compile-time options 
//////////////////////////////////////////////////////////////

//***Variable Definition values determined by software
int temperature = 70;  // Stores the most recent valid temperature
int humidity = 50;   // Stores the most recent valid relative humidity
String hum="--",tem="--",pres="--"; //adjusted values "--" if nan or current reading if valid
#define pinBoardLED 2   //onboard LED

//***Temperature/Humidity/Pressure Sensor Libraries
#ifdef AHT10_
  #include <Adafruit_AHT10.h>
  Adafruit_AHT10 aht;
#endif
#ifdef DHT_
  #include <Adafruit_Sensor.h>
  #define pinDHT 5                 
  #include "DHT.h"
  #define DHTTYPE DHT11  //or DHT21, DHT22
  DHT dht(pinDHT,DHTTYPE);
#endif  
#ifdef BME_
  #include <Adafruit_Sensor.h>
  #include <Wire.h>               //12C bus library
  #include <Adafruit_BME280.h>    // library for BME280 temp hum pressure sensor
  Adafruit_BME280 bme;            // I2C
#endif
#ifdef DS18B20_
  #include <OneWire.h>
  #include <DallasTemperature.h>
  
  // GPIO where the DS18B20 is connected to
  const int oneWireBus = 4;     
  
  // Setup a oneWire instance to communicate with any OneWire devices
  OneWire oneWire(oneWireBus);
  
  // Pass our oneWire reference to Dallas Temperature sensor 
  DallasTemperature sensors(&oneWire);
#endif
#ifdef SHT20_  
  #include "DFRobot_SHT20.h"
  DFRobot_SHT20 sht20(&Wire, SHT20_I2C_ADDR);
#endif    
    
//***Wifi libraries
#ifdef WIFI_
  #include <ESP8266WiFi.h> 
  #define WIFI_SSID SECRET_WIFI_SSID  
  #define WIFI_PASSWORD SECRET_WIFI_PASSWORD
  WiFiClient client;
#endif

//***MQTT libraries  and parameters, topics
#ifdef MQTT_
  #include <PubSubClient.h>
  PubSubClient mqttClient(client);
  #define MQTT_USER_NAME SECRET_MQTT_USER_NAME         
  #define MQTT_PASSWORD SECRET_MQTT_USER_PWD            
  #define MQTT_SERVER SECRET_MQTT_HOST
  #define DEVICE_NAME "device1" 
  #define LWT_TOPIC "device1/status/LWT"             // MQTT Last Will & Testament
  #define TEMPERATURE_TOPIC "device1/temperature"
  #define HUMIDITY_TOPIC "device1/humidity"
  #define VERSION_TOPIC "device1/report/version"     // report software version at connect
  #define MSG_BUFFER_SIZE 512                        // for MQTT message payload (growth)
  char msg[MSG_BUFFER_SIZE];
  unsigned long lastReconnectAttempt = 0;
  unsigned long lastPublish = 0, lastRead = 0, lastPressErrReport = 0;
  unsigned long lastPublishNow, mqttNow; //
#endif

//***ThingSpeak library
#ifdef THINGSPEAK_
  #include <ThingSpeak.h> // always include thingspeak header file after other header files and custom macros
#endif

//////////////////////////////////////////////////////////////  
//        Program software in alphabetical order except
//        Setup() and Loop() are at the end 
//////////////////////////////////////////////////////////////

boolean mqttReconnect()
{
  #ifdef MQTT_  
    Serial.println();
    Serial.println(F("*****mqttReconnect()***** "));
    if (mqttClient.connect(DEVICE_NAME, MQTT_USER_NAME, MQTT_PASSWORD, LWT_TOPIC, 2, true, "Disconnected"))
    {
      Serial.print(F("MQTT connected to "));
      Serial.println(F(MQTT_SERVER));
  
      // Publish MQTT announcements...
      mqttClient.publish(LWT_TOPIC, "Connected", true); // let broker know we're connected
      Serial.printf("MQTT SENT: %s/Connected\n", LWT_TOPIC);
      mqttClient.publish(VERSION_TOPIC, VERSION, true); // report firmware version
      Serial.printf("MQTT SENT: Firmware %s\n", VERSION);
      }
    return mqttClient.connected();
  #endif  
}
  
//*********************************** 
void publishMQTT(){
  #ifdef MQTT_
    Serial.println();
    Serial.println(F("*****publishMQTT()***** "));
    //***verify mqtt connection
    if (!mqttClient.connected())
    {
      mqttNow = millis();
      if (mqttNow - lastReconnectAttempt > 1000)
      {
        Serial.println(F("Waiting for MQTT..."));
        lastReconnectAttempt = mqttNow;
        // Attempt to reconnect
        if (mqttReconnect())
        {
          lastReconnectAttempt = 0;
        }
      }
    }
    else
    {
      // Client connected
      mqttClient.loop();
    }
  
    //****Publish MQTT
    lastPublishNow = millis();
    if(mqttClient.connected() )
    {
      //sprintf(msg, "%.2f", temperature);
      sprintf(msg, "%d", temperature);
      mqttClient.publish(TEMPERATURE_TOPIC, msg, true);
      Serial.print(TEMPERATURE_TOPIC); Serial.println(F(" published"));
      
      //sprintf(msg, "%.2f", humidity);
      sprintf(msg, "%d", humidity);
      mqttClient.publish(HUMIDITY_TOPIC, msg, true);
      Serial.print(HUMIDITY_TOPIC);Serial.println(F(" published"));
      
      lastPublish = millis(); 
    }      
  #endif 
}

//*********************************
void publishThingSpeak()  
{
  //****Publish to Thingspeak
  #ifdef THINGSPEAK_
    #ifdef DEBUG_
      Serial.println();
      Serial.println(F("*****publishThingSpeak()***** "));
    #endif
/*        
    Parameters:
    channelNumber - Channel number
    field - Field number (1-8) within the channel to write to.
    value - data
    writeAPIKey - Write API key associated with the channel.
    
    Returns:
    HTTP status code of 200 if successful.
*/
    // Write temperature to field 1.
    int x = ThingSpeak.writeField(SECRET_AQI_THINGSPEAK_CHANNEL_Device, 1, temperature, SECRET_AQI_THINGSPEAK_API_KEY_Device);
    #ifdef DEBUG_
      if(x == 200){
        Serial.println(F("ThingSpeak temperature (field 1) update successful."));
      }
      else{
        Serial.println("ThingSpeak problem updating temperature (field 1). HTTP error code " + String(x));
      }
    #endif
    delay(20000);   //Wait 20 seconds between data writes to ThingSpeak!!

    // Write humidity to field 2.
    x = ThingSpeak.writeField(SECRET_AQI_THINGSPEAK_CHANNEL_Device, 2, humidity, SECRET_AQI_THINGSPEAK_API_KEY_Device);
    #ifdef DEBUG_
      if(x == 200){
        Serial.println(F("ThingSpeak humidity (field 2) update successful."));
      }
      else{
        Serial.println("ThingSpeak problem updating humidity (field 2). HTTP error code " + String(x));
      }  
    #endif  
  #endif
}

//***********************************
void readHumidity() {  
  //valid data updates humidity and hum; invalid updates only hum with  "--"
  Serial.println();
  Serial.println(F("*****readHumidity()***** "));
  float x=0;
  #ifdef AHT10_
    sensors_event_t humidity, temp;
    aht.getEvent(&humidity, &temp);// populate temp and humidity objects with fresh data
    x=humidity.relative_humidity;  
  #endif  
  #ifdef DHT_
    x = (dht.readHumidity());
  #endif
  #ifdef BME_
    x = (bme.readHumidity());
  #endif
  #ifdef SHT20_
    x = (sht20.readHumidity());  
  #endif
  hum=String(x);
  if (hum == "nan") {hum = "--";
   }else{humidity = round(x);
         hum=humidity;
  }
  Serial.print(F("humidity, hum: "));Serial.print(humidity);Serial.print(F(", "));Serial.println(hum);
}

//***********************************
void readTemperature() {  
  //valid data updates temperature and temp; invalid updates only temp with  "--"
  Serial.println();
  Serial.println(F("*****readTemperature()***** "));
  float x = 0; 
  #ifdef AHT10_
    sensors_event_t humidity, temp;
    aht.getEvent(&humidity, &temp);// populate temp and humidity objects with fresh data
    x=temp.temperature;
    x=1.8*x+32.0;
  #endif 
  #ifdef DHT_
    x=dht.readTemperature(true);  //true = *F
  #endif
  #ifdef BME_
    x=1.8*bme.readTemperature()+32.0;  //Temperature degrees F 
  #endif     
  #ifdef DS18B20_
    sensors.requestTemperatures(); 
    //x = sensors.getTempCByIndex(0);
    x = sensors.getTempFByIndex(0);
  #endif 
  #ifdef SHT20_
    x = 1.8*sht20.readTemperature()+32.0;
  #endif
  tem=String(x);
  if (tem=="nan"){tem = "--";
  }else{temperature = round(x);
        tem=temperature;
  }
  Serial.print(F("temperature, tem: "));Serial.print(temperature);Serial.print(F(", "));Serial.println(tem);
}

//*********************************
void setupMQTT()
{
  #ifdef MQTT_
    Serial.println();
    Serial.println(F("*****setupMQTT()*****"));
    mqttClient.setBufferSize(MSG_BUFFER_SIZE);
    mqttClient.setServer(MQTT_SERVER, 1883);
    lastReconnectAttempt = 0;
    Serial.println(F("setupMQTT completed"));
  #endif
}
  
//*********************************
void setupSensors(){
  //initializes sensors
    Serial.println();
    Serial.println(F("*****setupSensors()*****"));
  #ifdef AHT10_
    aht.begin();
    delay(100);
    Serial.println(F("AHT10 sensor has been initialized"));   
  #endif 
  #ifdef BME_
    #define SEALEVELPRESSURE_HPA (1013.25)
    bool bme280=bme.begin(0x76);
    delay(100);
    if (!bme280){
       Serial.println(F("Failed to initiate bme280"));
    }else{
      Serial.println(F("BME280 sensor has been initialized"));   
    }
  #endif
  #ifdef DHT_
    dht.begin();
    delay(1000);
    Serial.println(F("DHTxx sensor has been initialized"));   
  #endif
  #ifdef DS18B20_
    sensors.begin();
  #endif  
  #ifdef MCP9808_      
    if (!tempsensor.begin()) {
       Serial.println(F(" Couldn't find the MCP9808!"));
    }else{
      Serial.println(F("MCP9808 sensor has been initialized"));   
    } 
  #endif
  #ifdef SHT20_
    sht20.initSHT20();
    delay(100);
    sht20.checkSHT20();
    Serial.println(F("SHT20 sensor has been initialized"));    
 #endif   
}

//*********************************
void setupThingSpeak()
{
  #ifdef THINGSPEAK_
    Serial.println();
    Serial.println(F("*****setupThingSpeak()*****"));
    ThingSpeak.begin(client); 
    Serial.println(F("setupThingSpeak completed"));
  #endif
}

//*********************************
void setupWiFi()
{
  //***Setup wifi
  #ifdef WIFI_
    Serial.println();
    Serial.println(F("*****setupWiFi()*****"));
    Serial.println("Connecting.. ");
    Serial.println(WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.println("WIFI connected");
  #endif    
} 
 
//************************************
void sleep(uint64_t seconds)
{ 
  Serial.println();
  Serial.println(F("*****sleep()***** "));
  uint64_t uS_TO_S_FACTOR = 1000000;  // Conversion factor for micro seconds 
  Serial.print(F("Sleeping "));Serial.print(seconds); Serial.println(F(" seconds zzzzz...."));  //Go into deep sleep                      
  ESP.deepSleep(seconds * uS_TO_S_FACTOR); 
}


//***********************
//   **  setup()  **
//***********************

void setup()
{
  delay(200);
  Serial.begin(115200);    // Initialize the serial monitor
  delay(2000);             // Give user time open the Serial Monitor if chosen
  Serial.println(VERSION); // Print the application and Version
  Serial.println();        // Create a blank line
  Serial.println(F("_____setup()_____ "));
  Serial.println();        // Create a blank line
  Serial.println("deviceID = " + String(deviceID));
       
  pinMode(pinBoardLED,OUTPUT);      // set the board LED pin to output mode
  digitalWrite(pinBoardLED, LOW);   //illuminate the on-board LED

  setupSensors();         // Initialize temperature/humidity sensor in the enabled #define statement above
  setupWiFi();            // Initialize Wifi if #define WIFI_ is enabled 
  setupMQTT();            // Initialize MQTT if #define MQTT_ is enabled
  setupThingSpeak();      // Initialize ThingSpeak if #define THINGSPEAK_ is enabled
  
  delay(1000);
  digitalWrite(pinBoardLED, HIGH);  // Turn off board LED        
}
 
//***********************
//   **  loop()  **
//***********************

void loop()
{    
  Serial.println();       // Create a blank line
  Serial.println(F("_____Loop()_____"));
  
  readTemperature();      // Read the temperature sensor in the enabled #define statement above
  readHumidity();         // Read the humidity sensor in the enabled #define statement above
  publishMQTT();          // publish MQTT if #define MQTT_ is enabled
  publishThingSpeak();    // publish ThingSpeak if #define THINGSPEAK_ is enabled
  sleep(sleepMinutes*60); // deep sleep time in seconds
}
