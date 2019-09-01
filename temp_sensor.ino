#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <splash.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <gfxfont.h>
#include <DHTesp.h>
#include <BearSSLHelpers.h>
#include <CertStoreBearSSL.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiAP.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiSTA.h>
#include <ESP8266WiFiType.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <WiFiClientSecureAxTLS.h>
#include <WiFiClientSecureBearSSL.h>
#include <WiFiServer.h>
#include <WiFiServerSecure.h>
#include <WiFiServerSecureAxTLS.h>
#include <WiFiServerSecureBearSSL.h>
#include <WiFiUdp.h>
#include <ESP8266HTTPClient.h>
#include <Adafruit_MCP9808.h>
#include <Wire.h>


#define h_sens 0
#define heating 16
const char* ssid = "Maroc";
const char* password = "marocmarocko";
float tempTarget = 37.85;
int currPWM = 0;

Adafruit_SSD1306 display(-1);
// Create the MCP9808 temperature sensor object
Adafruit_MCP9808 tempsensor = Adafruit_MCP9808();
DHTesp dht;


void setup() {
  pinMode(heating, OUTPUT);
  // put your setup code here, to run once:
  Serial.begin(115200);

  //DISPLAY INIT
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();  //Pulisce il buffer da inviare al display
  display.setTextSize(2);  //Imposta la grandezza del testo
  display.setTextColor(WHITE); //Imposta il colore del testo (Solo bianco)
  
  WiFi.begin(ssid, password);
  Serial.print("Connecting..");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  while (!Serial); //waits for serial terminal to be open, necessary in newer arduino boards.
  if (!tempsensor.begin(0x18)) {
    Serial.println("Couldn't find MCP9808! Check your connections and verify the address is correct.");
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0,0);
    display.print("Sensor not found!");
    display.setCursor(0,15);
    display.print("Please reboot.");
    while (1);
  }
  
  Serial.println("Found MCP9808!");
  tempsensor.setResolution(3); // sets the resolution mode of reading, the modes are defined in the table bellow:
  // Mode Resolution SampleTime
  //  0    0.5°C       30 ms
  //  1    0.25°C      65 ms
  //  2    0.125°C     130 ms
  //  3    0.0625°C    250 ms
  Serial.println("Setup DHT11 to gpio0");
  dht.setup(h_sens, DHTesp::DHT11);
}



void loop() {
  // Read and print out the temperature, also shows the resolution mode used for reading.
  float c = tempsensor.readTempC();
  delay(290);
  float c2 = tempsensor.readTempC();
  delay(290);
  float c3 = tempsensor.readTempC();
  delay(290);
  c = (c+c2+c3)/3;
  
  //Activate heating if temp is lower
  if(c < (tempTarget - 0.5))
  {
    currPWM = 500;
    analogWrite(heating, currPWM);
  }
  if ((c >= (tempTarget - 0.5)) && (c < tempTarget))
  {
    currPWM = 400;
    analogWrite(heating, currPWM);
  }
  if(c >= tempTarget)
  {
    currPWM = 0;
    analogWrite(heating, currPWM);
  }
  
  delay(dht.getMinimumSamplingPeriod());
  float humidity = dht.getHumidity();
  //Serial.print(humidity, 1); Serial.print(" %\n");

  
  
  if(WiFi.status()== WL_CONNECTED){   //Check WiFi connection status
    HTTPClient http;    //Declare object of class HTTPClient
    http.begin("http://94.23.71.222/ale-arduino/upload.php?temp=" + String(c));      //Specify request destination
    http.addHeader("Content-Type", "text/plain");  //Specify content-type header
    int httpCode = http.POST("Message from ESP8266");   //Send the request
    String payload = http.getString();                  //Get the response payload
    http.end();  //Close connection
  }else{
    Serial.println("Error in WiFi connection");
  }

  display.clearDisplay();
  display.setCursor(0,0); //Imposta la posizione del cursore (Larghezza,Altezza)
  display.print("T:"); //Stringa da visualizzare
  display.print(c, 2);
  display.print(" C");
  display.setCursor(0,15);
  display.print("H:"); //Stringa da visualizzare
  display.print(humidity, 0);
  display.print("% ");
  display.print(currPWM);
  display.display(); //Invia il buffer da visualizzare al display
}
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();  //Pulisce il buffer da inviare al display
  display.setTextSize(2);  //Imposta la grandezza del testo
  display.setTextColor(WHITE); //Imposta il colore del testo (Solo bianco)
  
  WiFi.begin(ssid, password);
  Serial.print("Connecting..");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  while (!Serial); //waits for serial terminal to be open, necessary in newer arduino boards.
  if (!tempsensor.begin(0x18)) {
    Serial.println("Couldn't find MCP9808! Check your connections and verify the address is correct.");
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0,0);
    display.print("Sensor not found!");
    display.setCursor(0,15);
    display.print("Please reboot.");
    while (1);
  }
  
  Serial.println("Found MCP9808!");
  tempsensor.setResolution(3); // sets the resolution mode of reading, the modes are defined in the table bellow:
  // Mode Resolution SampleTime
  //  0    0.5°C       30 ms
  //  1    0.25°C      65 ms
  //  2    0.125°C     130 ms
  //  3    0.0625°C    250 ms
  Serial.println("Setup DHT11 to gpio0");
  dht.setup(h_sens, DHTesp::DHT11);
}



void loop() {
  // Read and print out the temperature, also shows the resolution mode used for reading.
  float c = tempsensor.readTempC();
  delay(290);
  float c2 = tempsensor.readTempC();
  delay(290);
  float c3 = tempsensor.readTempC();
  delay(290);
  c = (c+c2+c3)/3;
  
  //Activate heating if temp is lower
  if(c < tempTarget)
  {
    currPWM = 400;
    analogWrite(heating, currPWM);
  }
  else
  {
    currPWM = 0;
    analogWrite(heating, currPWM);
  }
  
  delay(dht.getMinimumSamplingPeriod());
  float humidity = dht.getHumidity();
  //Serial.print(humidity, 1); Serial.print(" %\n");

  
  
  if(WiFi.status()== WL_CONNECTED){   //Check WiFi connection status
    HTTPClient http;    //Declare object of class HTTPClient
    http.begin("http://**.**.**.**/ale-arduino/upload.php?temp=" + String(c));      //Specify request destination
    http.addHeader("Content-Type", "text/plain");  //Specify content-type header
    int httpCode = http.POST("Message from ESP8266");   //Send the request
    String payload = http.getString();                  //Get the response payload
    http.end();  //Close connection
  }else{
    Serial.println("Error in WiFi connection");
  }

  display.clearDisplay();
  display.setCursor(0,0); //Imposta la posizione del cursore (Larghezza,Altezza)
  display.print("Temp:"); //Stringa da visualizzare
  display.print(c, 2);
  display.setCursor(0,15);
  display.print("Um:"); //Stringa da visualizzare
  display.print(humidity, 0);
  display.print("  ");
  display.print(currPWM);
  display.display(); //Invia il buffer da visualizzare al display
}
