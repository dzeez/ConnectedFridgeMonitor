// ask_reliable_datagram_client.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple addressed, reliable messaging client
// with the RHReliableDatagram class, using the RH_ASK driver to control a ASK radio.
// It is designed to work with the other example ask_reliable_datagram_server
// Tested on Arduino Mega, Duemilanova, Uno, Due, Teensy

#include <RHReliableDatagram.h>
#include <RH_ASK.h>
#include <SPI.h>
#include <stdlib.h>

#include <OneWire.h>
#include <DallasTemperature.h>
// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS A1
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

#include "DHT.h"
#define DHTPIN 3
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

#define CLIENT_ADDRESS 1
#define SERVER_ADDRESS 2

// Singleton instance of the radio driver
RH_ASK driver;

// Class to manage message delivery and receipt, using the driver declared above
RHReliableDatagram manager(driver, CLIENT_ADDRESS);

void setup() 
{
  //for ds18b20
  pinMode(A0,OUTPUT);
  pinMode(A2,OUTPUT);
  digitalWrite(A0,HIGH);
  digitalWrite(A2,LOW);
  
  /* for DHT sensor
      pinMode(2, OUTPUT);
  digitalWrite(2,HIGH);
  pinMode(4, OUTPUT);
  digitalWrite(4,LOW);
  pinMode(5, OUTPUT);
  digitalWrite(5,LOW);
  */
  
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH); //for xmitter
  
  Serial.begin(115200);
  if (!manager.init())
    Serial.println("init failed");
  else Serial.println("Transmit. Driver initialized");
  
  dht.begin();
  sensors.begin();
  delay(1000);
}


//uint8_t data[] = "Hello World!";
// Dont put this on the stack:
uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];

void loop()
{
  Serial.println("Sending to ask_reliable_datagram_server");
  sensors.requestTemperatures(); // Send the command to get temperatures
  char buffer[10];
  //char *data = dtostrf(dht.readTemperature(true),5,2,buffer);
  char *data = dtostrf(sensors.getTempFByIndex(0),5,2,buffer);
  
  Serial.println(data);
  //Serial.println(sizeof(dht.readTemperature(true)));
  //Serial.println(strlen(data));
  // Send a message to manager_server
  //if (manager.sendtoWait((uint8_t *)data, sizeof(data), SERVER_ADDRESS)) //dsd
  if (manager.sendtoWait((uint8_t *)data, strlen(data), SERVER_ADDRESS))
  {
    /* dsd - not needed; these things are simplex!
    // Now wait for a reply from the server
    uint8_t len = sizeof(buf);
    uint8_t from;   
    if (manager.recvfromAckTimeout(buf, &len, 2000, &from))
    {
      Serial.print("got reply from : 0x");
      Serial.print(from, HEX);
      Serial.print(": ");
      Serial.println((char*)buf);
    }
    else
    {
      Serial.println("No reply, is ask_reliable_datagram_server running?");
    }
    */
  }
  else
    Serial.println("sendtoWait failed");
  //delay(2000);//for dht
  delay(250); //FIXME: adjust?
}

