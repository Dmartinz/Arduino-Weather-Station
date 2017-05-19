/*
    Thingspeak API Upload for BME 280 and Anemometer
*/
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <ESP8266WiFi.h>
#include <ThingSpeak.h>

#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10

#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME280 bme;
unsigned long delayTime;

#include <ESP8266WiFi.h>

const int analogInPin = A0;
int sensorValue = 0;
float sensorVoltage = 0.0;
float Windspeed = 0;

//Wifi Host *UPDATE THIS WITH YOU OWN INFORMATION
const char* ssid     = "Hotspot";
const char* password = "Password";

WiFiClient client;

//Server Information
const char* host = "api.thingspeak.com";
const int postingInterval = 30 * 1000; // post data every 30 seconds to prevent missing points

// ThingSpeak Temperature Channel
const int TempchannelID = 252947;
String TempwriteAPIKey = "8MQNBPETGJFHQTET"; // write API key for your ThingSpeak Channel

// ThingSpeak Humidity Channel
const int HumchannelID = 252949;
String HumwriteAPIKey = "C7TM5S2GLDSG3YV6"; // write API key for your ThingSpeak Channel

// ThingSpeak Pressure Channel
const int PrechannelID = 255500;
String PrewriteAPIKey = "LIOXGIYPYUOR3VSU"; // write API key for your ThingSpeak Channel

// ThingSpeak Wind Channel
const int WindchannelID = 265952;
String WindwriteAPIKey = "PYE9PHFT8BYUQT0I"; // write API key for your ThingSpeak Channel

void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println(F("BME280 test + Wifi + Thingspeak"));

  bool status;

  // default settings
  status = bme.begin();
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }

  Serial.println("-- Default Test --");
  delayTime = 5000;

  Serial.println();

  // Now we connect to a WiFi network
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.print("");
  Serial.print("WiFi connected");
  Serial.println();
  Serial.print("IP address: ");
  Serial.println();
  Serial.print(WiFi.localIP());
  Serial.println();

  int value = 0;
  delay(5000);
  ++value;

  Serial.print("connecting to ");
  Serial.println(host);
  Serial.println();
}

void loop() {
  if (client.connect(host, 80)) {

    // Measure Signal Strength (RSSI) of Wi-Fi connection- Collect values
    float rssi = bme.readTemperature();
    float rssi2 = bme.readHumidity();
    float rssi3 = bme.readPressure() / 100;

    // Show values that were measured
    Serial.print('\n');
    Serial.println("Weather Values");
    printValues();
    Wind();
    float rssi4 = Windspeed;

    //Create String using value
    String body = "field1=";
    body += String(rssi, 2);
    //Create String using value
    String body2 = "field1=";
    body2 += String(rssi2, 2);
    //Create String using value
    String body3 = "field1=";
    body3 += String(rssi3, 2);
    String body4 = "field1=";
    body4 += String(rssi4, 2);

//We will now post to Thingspeak
    Serial.print("Posting to Thingspeak");
    Serial.println();

 
    //Post on the Temperature channel
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + TempwriteAPIKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(body.length());
    client.print("\n\n");
    client.print(body);
    client.print("\n\n");



    // Post on Humidity Channel
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + HumwriteAPIKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(body2.length());
    client.print("\n\n");
    client.print(body2);
    client.print("\n\n");



    // Post on Pressure Channel
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + PrewriteAPIKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(body3.length());
    client.print("\n\n");
    client.print(body3);
    client.print("\n\n");


    // Post on Windspeed Channel
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + WindwriteAPIKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(body4.length());
    client.print("\n\n");
    client.print(body4);
    client.print("\n\n");


    delay(postingInterval);

    Serial.println("Done Posting to Thingspeak");
    // wait and then post again

  }
  client.stop();
}

//Windspeed function
void Wind() {
  sensorValue = analogRead(analogInPin);
  sensorVoltage = sensorValue * .00322266; // Convert from 0...1024 to 0...5v
  //Serial.println("Sensor Value: ");
  //Serial.print(sensorValue);
  //Serial.println("Sensor Voltage: ");
  //Serial.print(sensorVoltage);

  Windspeed = ((sensorVoltage - .66) / (.04969136)) * 2.23694;

  if (Windspeed >= 0) {
    Serial.print("WindSpeed = ");
    Serial.print(Windspeed);
    Serial.print(" mph");
    Serial.println();
  }

  if (Windspeed < 0) {
    Serial.print("WindSpeed = ");
    Serial.print(0);
    Windspeed = 0;
    Serial.print(" mph");
    Serial.println();
  }

}
//Sensor Function for printing all values 
void printValues() {
  Serial.print("Temperature = ");
  Serial.print(bme.readTemperature());
  Serial.println(" *C");

  Serial.print("Pressure = ");

  Serial.print(bme.readPressure() / 100);
  Serial.println(" hPa");

  Serial.print("Humidity = ");
  Serial.print(bme.readHumidity());
  Serial.println(" %");
}
