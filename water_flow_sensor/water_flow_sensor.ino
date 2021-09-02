/*
 Water flow sensor project
 Connected to ESP8266
 
 Data send to Thinger.io for visualisation
 Send notification to IFTTT when treshold reach

    BOARD: ESP8266-ESP12E
    BOARD-No1
    
    PIN
    SENSOR-->ESP(1)
    Black(GND)-->G(Pin beside 3V)//Working
    Red(VCC)-->VIN
    Yellow-->D4 (Working)
*/

#define _DISABLE_TLS_ //This will solve problem to connect to thinger.io server by disabling secure TLS/SSL
#define _DEBUG_ //For debugging
#include <ThingerESP8266.h> //For thinger.io (Please add library in Arduino IDE
#include "AnotherIFTTTWebhook.h" //For IFTTT: webhook please add this file in the same folder
#include <ESP8266WiFi.h>

//Thinger.io (You must have account with Thinger.io)
#define USERNAME "username"
#define DEVICE_ID "device_id"
#define DEVICE_CREDENTIAL "credential"

// Set IFTTT Webhooks event name and key
#define IFTTT_Key "your_key"
#define IFTTT_Event "your_event"

//Local wifi setting
#define SSID "YOUR_SSID"
#define SSID_PASSWORD "PASSWORD"

//Water flow sensor attached to esp8266(data-yellow)
#define SENSOR  D4 //working

//Connecting to thinger.io
ThingerESP8266 thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL);

long currentMillis = 0;
long previousMillis = 0;
int interval = 1000;

//boolean ledState = LOW;
float calibrationFactor = 4.5;

volatile byte pulseCount;
byte pulse1Sec = 0;

float flowRate;
char buff[10];
char valueString[20] = "";

char dest[30];


unsigned int flowMilliLitres;
unsigned long totalMilliLitres;


/*
//For lolin V3
void ICACHE_RAM_ATTR pulseCounter()
{
  pulseCount++;
}
*/


// for current esp8266 --> working
void IRAM_ATTR pulseCounter()
{
  pulseCount++;
}


void setup() {
  // put your setup code here, to run once:
   Serial.begin(115200);
  //thing.add_wifi(SSID, SSID_PASSWORD);
  WiFi.begin(SSID, SSID_PASSWORD);




  // Connecting to WiFi...
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


   
  
  pinMode(SENSOR, INPUT_PULLUP);
  //pinMode(SENSOR, INPUT);
  pulseCount = 0;
  flowRate = 0.0;
  flowMilliLitres = 0;
  totalMilliLitres = 0;
  previousMillis = 0;
  attachInterrupt(digitalPinToInterrupt(SENSOR), pulseCounter, FALLING);//Working
  //attachInterrupt(SENSOR, pulseCounter, FALLING);//Ubah (NOT NEEDED)
  

}

void loop() {
  // put your main code here, to run repeatedly:
   currentMillis = millis();
   
  if (currentMillis - previousMillis > interval) {
    //detachInterrupt(SENSOR);//Tambah (NOT NEEDED)
    pulse1Sec = pulseCount;
    pulseCount = 0;
    flowRate = ((1000.0 / (millis() - previousMillis)) * pulse1Sec) / calibrationFactor;
    previousMillis = millis();
    flowMilliLitres = (flowRate / 60) * 1000;
    totalMilliLitres += flowMilliLitres;
    
    // Print the flow rate for this second in litres / minute
    Serial.print("Flow rate: ");
    //Serial.print(int(flowRate));  // Print the integer part of the variable
    Serial.print(flowRate);
    Serial.println("L/min");
    //Serial.println();
    //Serial.print("\t");       // Print tab space

    //to send notification to IFTTT
    if(flowRate>30){ //Please adjust this

      //buff[10]="";
      //valueString = "";

      //dest="";

      // Send Webook to IFTTT
      dtostrf(flowRate, 4, 2, buff);  //4 is mininum width, 6 is precision
      //strcat(valueString, buff); --> I don't need this
      //strcat(valueString, ", ");
      strcpy( dest, buff );
      //strcat( dest, "L/min" );
      send_webhook(IFTTT_Event,IFTTT_Key,"Taufik, Water is flowing at Left/Short Arm",strcat( dest, "L/min" ),"now");
      //delay(120000);//To send notification every 2 minutes --> please adjust this
      //Check strcat & strcpy! concatentate look awkward - DONE.
      }
    
    // Print the cumulative total of litres flowed since starting
    Serial.print("Output Liquid Quantity: ");
    Serial.print(totalMilliLitres);
    Serial.print("mL / ");
    Serial.print(totalMilliLitres / 1000);
    Serial.println("L");
    Serial.println();
    //delay(500);
    
    thing["data"] >> [](pson& out){
      out["Flow Rate"] = flowRate;
      out["Total"]= totalMilliLitres;
      };
      
    thing.handle();
    thing.stream(thing["data"]);
    //attachInterrupt(SENSOR, pulseCounter, FALLING);//Tambah (NOT NEEDED)
  }

}
