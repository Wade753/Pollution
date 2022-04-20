#include <TinyGPS++.h>

#include <SoftwareSerial.h>

#include "ThingSpeak.h"

#include <ESP8266WiFi.h>

#include "MQ135.h"

static
const int RXPin = 4,
  TXPin = 5;
static
const uint32_t GPSBaud = 9600;
float latitude, longitude;
String lat_str, lng_str;
// repace your wifi username and password
const char * ssid = "VM4852933";
const char * password = "tv4hwVdpSjk6";
unsigned long myChannelNumber = 1543926;
const char * myWriteAPIKey = "HFTAQCPXIBVTQOH0";
// The TinyGPS++ object
TinyGPSPlus gps;
WiFiClient client;
// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);
void setup() {
  Serial.begin(115200);
  ss.begin(GPSBaud);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Netmask: ");
  Serial.println(WiFi.subnetMask());
  Serial.print("Gateway: ");
  Serial.println(WiFi.gatewayIP());
  ThingSpeak.begin(client);
}
void loop() {
    MQ135 gasSensor = MQ135(A0);
    float air_quality = gasSensor.getPPM();
    Serial.print("Air Quality: ");
    Serial.print(air_quality);
    Serial.println("  PPM");
    Serial.println();

    if (client.connect("api.thingspeak.com", 80)) // "184.106.153.149" or api.thingspeak.com
    {
      String postStr = apiKey;
      postStr += "&field1=";
      postStr += String(air_quality);
      postStr += "r\n";
      client.print("POST /update HTTP/1.1\n");
      client.print("Host: api.thingspeak.com\n");
      client.print("Connection: close\n");
      client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
      client.print("Content-Type: application/x-www-form-urlencoded\n");
      client.print("Content-Length: ");
      client.print(postStr.length());
      client.print("\n\n");
      client.print(postStr);

      Serial.println("Data Send to Thingspeak");

      while (ss.available() > 0) {
        if (gps.encode(ss.read())) {
          if (gps.location.isValid()) {
            latitude = gps.location.lat();
            lat_str = String(latitude, 6);
            longitude = gps.location.lng();
            lng_str = String(longitude, 6);
            Serial.print("Latitude = ");
            Serial.println(lat_str);
            Serial.print("Longitude = ");
            Serial.println(lng_str);
            ThingSpeak.setField(2, lat_str);
            ThingSpeak.setField(3, lng_str);
            ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
          }
          delay(1000);
          Serial.println();
        }
      }
    }
