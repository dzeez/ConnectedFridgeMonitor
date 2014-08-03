// ask_reliable_datagram_server.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple addressed, reliable messaging server
// with the RHReliableDatagram class, using the RH_ASK driver to control a ASK radio.
// It is designed to work with the other example ask_reliable_datagram_client
// Tested on Arduino Mega, Duemilanova, Uno, Due, Teensy

#include <RHReliableDatagram.h>
#include <RH_ASK.h>
#include <SPI.h>

#define CLIENT_ADDRESS 1
#define SERVER_ADDRESS 2

// Singleton instance of the radio driver
RH_ASK driver(2000,6,3,2);

// Class to manage message delivery and receipt, using the driver declared above
RHReliableDatagram manager(driver, SERVER_ADDRESS);

void setup() 
{
    pinMode(7, OUTPUT);
  digitalWrite(7, LOW);
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);
  
  Serial.begin(115200);
  if (!manager.init())
    Serial.println("init failed");
  else Serial.println("driver initialized");
}

uint8_t data[] = "And hello back to you";
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
    
    uint8_t from;
    if (manager.recvfromAck(buf, &len, &from))
    {
      Serial.print("got request from : 0x");
      Serial.print(from, HEX);
      Serial.print(": ");
      Serial.println((char*)buf);

      // Send a reply back to the originator client
      //if (!manager.sendtoWait(data, sizeof(data), from))
      //  Serial.println("sendtoWait failed");
    }
  }
}

