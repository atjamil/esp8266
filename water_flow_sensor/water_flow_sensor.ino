/*
 Soil moisture sensor project
 Connected to ESP8266
 
 Data send to Thinger.io for visualisation
 Send notification to IFTTT when treshold reach

     < 500 is too wet
    500-750 is the target range
    > 750 is dry enough to be watered

    Range: 315-1024

    BOARD: ESP8266-ESP12E
    BOARD-No2
    
    PIN
    SENSOR-->ESP(2)
    GND(-)-->GND
    VCC(+)-->D7
    DO-->
    AO-->A0

 
 */
#define _DISABLE_TLS_ //This will solve problem to connect to thinger.io server
#define _DEBUG_
#include <ThingerESP8266.h>
#include "AnotherIFTTTWebhook.h"
#include <ESP8266WiFi.h>

//Thinger.io
#define USERNAME "atjamil"
#define DEVICE_ID "esp8266_2"
#define DEVICE_CREDENTIAL "2014polar"

// Set IFTTT Webhooks event name and key
#define IFTTT_Key "dHXvG5qbGylRXe2PjHX8Py"
#define IFTTT_Event "ESP8266_SOIL_MOISTURE"

//Local wifi setting
#define SSID "atjamil@unifi"
#define SSID_PASSWORD "2014polar"

/* Change these values based on your calibration values */
#define soilWet 500   // Define max value we consider soil 'wet'
#define soilDry 750   // Define min value we consider soil 'dry'

//Soil moisture sensor attached to esp8266(data-yellow)
#define SENSORPIN  A0
#define SENSORPOWER D7

//Connecting to thinger.io
ThingerESP8266 thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL);

char buff[6];
char valueString[20] = "";

char dest[8];






void setup() {
  // put your setup code here, to run once:

    pinMode(SENSORPOWER, OUTPUT);
  
    // Initially keep the sensor OFF
    digitalWrite(SENSORPOWER, LOW);

  
   Serial.begin(115200);
   WiFi.begin(SSID, SSID_PASSWORD);

    // Connecting to WiFi...
    Serial.println();
    Serial.print("Connecting to... ");
    Serial.println(SSID);
    while (WiFi.status() != WL_CONNECTED)
      {
      delay(4000);
      Serial.println(".");
      }
 
    // Connected to WiFi
    Serial.println();
    Serial.println("Connected! IP address: ");
    Serial.println(WiFi.localIP());

  

}

void loop() {
    // put your main code here, to run repeatedly:
    //get the reading from the function below and print it
  //get the reading from the function below and print it
  int moisture = readSensor();
  Serial.print("Analog Output: ");
  Serial.println(moisture);

  // Determine status of our soil
  if (moisture < soilWet) {
    Serial.println("Status: Soil is too wet");
  } else if (moisture >= soilWet && moisture < soilDry) {
    Serial.println("Status: Soil moisture is perfect");
  } else {
    Serial.println("Status: Soil is too dry - time to water!");
  }
  
  delay(6000);  // Take a reading every second for testing
          // Normally you should take reading perhaps once or twice a day
  Serial.println();
    

    //to send notification to IFTTT
    if(moisture>soilDry){

      // Send Webook to IFTTT
      dtostrf(moisture, 4, 2, buff);  //4 is mininum width, 6 is precision
      //strcat(valueString, buff);
      //strcat(valueString, ", ");
      strcpy( dest, buff );
      Serial.print("moisture: ");
      Serial.println(moisture);
      Serial.print("buff: ");
      Serial.println(buff);
      Serial.print("dest: ");
      Serial.println(dest);
      //strcat( dest, "L/min" );
      send_webhook(IFTTT_Event,IFTTT_Key,"Soil moisture is too dry at:",dest,"Irrigation System: TRIGERRED");
      //send_webhook(IFTTT_Event,IFTTT_Key,"Taufik, Water is flowing at Left/Short Arm",strcat( dest, "L/min" ),"now");
      Serial.println("Enter Webhook");
      delay(10000);//To send notification every 2 minutes
      
      }
    

    
    thing["data1"] >> [](pson& out){
      out["Soil moisture"] = readSensor();
      
      };
      
    thing.handle();
    thing.stream(thing["dat1a"]);
  }

      //  This function returns the analog soil moisture measurement
int readSensor() {
  digitalWrite(SENSORPOWER, HIGH);  // Turn the sensor ON
  delay(10);              // Allow power to settle
  int val = analogRead(SENSORPIN);  // Read the analog value form sensor
  digitalWrite(SENSORPOWER, LOW);   // Turn the sensor OFF
  return val;             // Return analog moisture value
}
