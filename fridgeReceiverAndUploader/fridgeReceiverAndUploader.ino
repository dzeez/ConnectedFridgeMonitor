// fridgeReceiverAndUploader
// originally derived from: ask_reliable_datagram_server.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple addressed, reliable messaging server
// with the RHReliableDatagram class, using the RH_ASK driver to control a ASK radio.
// It is designed to work with the other example ask_reliable_datagram_client
// Tested on Arduino Mega, Duemilanova, Uno, Due, Teensy

#include <RHReliableDatagram.h>
#include <RH_ASK.h>
#include <SPI.h>
#include <Ethernet.h>
#include <HttpClient.h>
#include <Xively.h>

//#define CLIENT_ADDRESS 1
#define SERVER_ADDRESS 2

// Singleton instance of the radio driver
RH_ASK driver(2000,6,3,2); //dsd: customize pin config for "easypins" FIXME

// Class to manage message delivery and receipt, using the driver declared above
RHReliableDatagram manager(driver, SERVER_ADDRESS);

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
  //  XivelyDatastream(sensorId, strlen(sensorId), DATASTREAM_STRING),

};
// Finally, wrap the datastreams into a feed
XivelyFeed feed(100844, datastreams, 1 /* number of datastreams */);

EthernetClient client;
XivelyClient xivelyclient(client);


void setup() 
{
    pinMode(7, OUTPUT);
  digitalWrite(7, LOW);
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);
  
  Serial.begin(115200);
  if (!manager.init())
    Serial.println(F("init failed"));
  else Serial.println(F("Receiver. Driver initialized."));
  
  Serial.println(F("Starting single datastream upload to Xively..."));
  Serial.println();

  while (Ethernet.begin(mac) != 1)
  {
    Serial.println(F("Error getting IP address via DHCP, trying again..."));
    delay(15000);
  }
}

//uint8_t data[] = "And hello back to you";
// Dont put this on the stack:
uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];

void loop()
{
  //uint8_t buf[RH_ASK_MAX_MESSAGE_LEN]; //dsd test
  
  if (manager.available())
  {
    // Wait for a message addressed to us from the client
    uint8_t len = sizeof(buf);
      //  Serial.print("buf size: ");
    //Serial.println();
    
    //uint8_t from;
//    if (manager.recvfromAck(buf, &len, &from))
    if (manager.recvfromAck(buf, &len))
    {
      //char buffer[8];
      //Serial.print(F("got request from : 0x"));
      //Serial.print(from, HEX);
      //Serial.print(": ");
      //float sensorValue = 77.22F;
      //float sensorValue = String((char*)buf).toFloat();
      //Serial.println((char*)buf);
  //float sensorValue = sensors.getTempFByIndex(0);
  float sensorValue = atof((char*)buf);
    datastreams[0].setFloat(sensorValue);
 // datastreams[0].setFloat(sensorValue);

  Serial.print(F("Read sensor value: "));
  Serial.println(datastreams[0].getFloat());

  Serial.println(F("Uploading it to Xively"));
  int ret = xivelyclient.put(feed, xivelyKey);
  Serial.print(F("xivelyclient.put returned "));
  Serial.println(ret);

  Serial.println();
  delay(15000);

      // Send a reply back to the originator client
      //if (!manager.sendtoWait(data, sizeof(data), from))
      //  Serial.println("sendtoWait failed");
    }
  }
}

