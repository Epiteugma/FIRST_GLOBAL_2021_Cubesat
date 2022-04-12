#include <SPI.h>
#include <WiFi101.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Arduino_MKRENV.h>
#include <ArduCAM.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include "memorysaver.h"
#include <Seeed_HM330X.h>

WiFiSSLClient client;
char ssid[] = "SSID";        // your network SSID (name)
char pass[] = "PASSWORD";    // your network password 
char server[] = "api.z3db0y.com"; // Your api server that will handle the uploaded data
char apiToken[] = "J_!6tkh0s_Ts|!GU!b5V1U0|FKy4^T@A19WQfpuxcMaN8RfuV?n&7*9ey_T!IuOsYBU#VTwjrznPgISnKxlcg_qGO*101lv12TA3EE|6C|xBv!pm_5u6nDD4*KB-?eXn"; // Not operating anymore don't even try to use this
int status = WL_IDLE_STATUS;

const int CS = 7;
#define SD_CS 9

// TEMPERATUSE SENSOR PORT.
#define TEMPERATURE_SENSOR_PIN 2
OneWire oneWire(TEMPERATURE_SENSOR_PIN);
DallasTemperature dallasSensors(&oneWire);
HM330X particle;

void setup()
{
  Serial.begin(9600);
//  while(!Serial)
//  {
  if (!ENV.begin()) {
    Serial.println("Failed to initialize MKR ENV shield!");
    while (1);
  }
  //}
  setupWiFi(ssid, pass);
  dallasSensors.begin();
}

//==================================
//
//  IMPORTANT!
//  MAIN LOOP CODE!
//
//==================================

void loop()
{
  String temp = String((readTemp() + readEnvShieldTemp())/2);
  String sound = String(readVolume());
  String uv = String(readUV());
  String humidity = String(readEnvShieldHumidity());
  String pressure = String(readEnvShieldPressure());
  String illuminance = String(readEnvShieldIlluminance());
  uploadAPIdata(temp, sound, uv, humidity, pressure, illuminance);
}

//==================================
//
//  Sensor read code. SET PORTS HERE
//
//==================================

float readTemp()
{
  dallasSensors.requestTemperatures();
  float temp = dallasSensors.getTempCByIndex(0);
  if(temp == DEVICE_DISCONNECTED_C)
  {
    return 0;
  } else
  {
    return temp;
  }
}

float readUV()
{
  // UV SENSOR PORT.
  return analogRead(A6);
}

float readVolume()
{
  // SOUND SENSOR (Envelope) PORT.
  return analogRead(A5);
}

float readEnvShieldTemp(){
  // Temprature sensor on the shield.
  float temperature = ENV.readTemperature();
    return temperature;
}

float readEnvShieldHumidity(){
  // Humidity sensor on the shield.
  float humidity = ENV.readHumidity();
    return humidity;
}

float readEnvShieldPressure(){
  // Pressure sensor on the shield.
  float pressure = ENV.readPressure();
    return pressure;
}

float readEnvShieldIlluminance(){
  // Illuminance sensor on the shield.
  float illuminance = ENV.readIlluminance();
    return illuminance;
}

//==================================
//
//  INTERNAL WiFi CODE!
//  Only modify tokens, addresses.
//
//==================================

// Setup WiFi
void setupWiFi(char ssid[], char password[]) 
{
  // Check for shield.
  if(WiFi.status() == WL_NO_SHIELD) {
    Serial.println("No WiFi shield connected on the arduino board.");
    return;
  }

  // Code partially copied from example.
  int status = WL_IDLE_STATUS;
  Serial.print("Attempting to connect to" + ssid+": ");
  Serial.println(ssid);
  // Try connect to WiFi network.
  while(status != WL_CONNECTED)
  {
    status = WiFi.begin(ssid, password);
    delay(2000);
  }
  Serial.println("Connected to " + String(ssid));
}

boolean uploadAPIdata(String temp, String sound, String uv, String humidity, String pressure, String illuminance)
{
  // Enter API token here.
  // you need to flash this website on your arduino board or else it won't work.
  if(client.connect(server, 443))
  {
    Serial.println(temp + " " + sound + " " + uv + " " + humidity + " " + pressure + " " + illuminance);
    String endpoint = "POST /v1/cubesat?temp=" + temp + "&sound=" + sound + "&uv=" + uv + "&humidity=" + humidity + "&pressure=" + pressure + "&illuminance=" + illuminance + " HTTP/1.1";
    Serial.println(endpoint);
    client.println(endpoint);
    client.println("Host: "+server);
    client.println("Authorization: Bearer " + String(apiToken));
    client.println("Connection: close");
    client.println("");
    return true;
  }
  else
  {
    Serial.println("Failed to connect to "+server);
    return false;
  }
}
