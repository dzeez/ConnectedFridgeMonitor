#include <SPI.h>
#include <Ethernet.h>
#include <HttpClient.h>
#include <Xively.h>

#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS A1
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

#include "DHT.h"
#define DHTPIN 3
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// MAC address for your Ethernet shield
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEF };

// Your Xively key to let you upload data
char xivelyKey[] = "SjAlsvhDhyvZhEFYxzNK2-6Ds0CSAKx4V3lKdTZSczJ4RT0g";

// Analog pin which we're monitoring (0 and 1 are used by the Ethernet shield)
//int sensorPin = 2;

// Define the strings for our datastream IDs
char sensorId[] = "temptemp";
XivelyDatastream datastreams[] = {
  XivelyDatastream(sensorId, strlen(sensorId), DATASTREAM_FLOAT),
};
// Finally, wrap the datastreams into a feed
XivelyFeed feed(100844, datastreams, 1 /* number of datastreams */);

EthernetClient client;
XivelyClient xivelyclient(client);

void setup() {
    pinMode(A0,OUTPUT);
  pinMode(A2,OUTPUT);
  digitalWrite(A0,HIGH);
  digitalWrite(A2,LOW);
  
        pinMode(2, OUTPUT);
  digitalWrite(2,HIGH);
  pinMode(4, OUTPUT);
  digitalWrite(4,LOW);
  pinMode(5, OUTPUT);
  digitalWrite(5,LOW);
  
  // put your setup code here, to run once:
  Serial.begin(115200);
  
  Serial.println("Starting single datastream upload to Xively...");
  Serial.println();

  while (Ethernet.begin(mac) != 1)
  {
    Serial.println("Error getting IP address via DHCP, trying again...");
    delay(15000);
  }
  
  dht.begin();
}

void loop() {
  //int sensorValue = analogRead(sensorPin);
  sensors.requestTemperatures();
  //float sensorValue = dht.readTemperature(true);
  float sensorValue = sensors.getTempFByIndex(0);
  datastreams[0].setFloat(sensorValue);

  Serial.print("Read sensor value ");
  Serial.println(datastreams[0].getFloat());

  Serial.println("Uploading it to Xively");
  int ret = xivelyclient.put(feed, xivelyKey);
  Serial.print("xivelyclient.put returned ");
  Serial.println(ret);

  Serial.println();
  delay(15000);
}
