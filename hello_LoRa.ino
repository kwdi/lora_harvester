// Hello LoRa - ABP TTN Packet Sender (Multi-Channel)
// Tutorial Link: https://learn.adafruit.com/the-things-network-for-feather/using-a-feather-32u4
//
// Adafruit invests time and resources providing this open source code.
// Please support Adafruit and open source hardware by purchasing
// products from Adafruit!
//
// Copyright 2015, 2016 Ideetron B.V.
//
// Modified by Brent Rubell for Adafruit Industries, 2018
/************************** Configuration ***********************************/
#include <TinyLoRa.h>
#include <SPI.h>

// Visit your thethingsnetwork.org device console
// to create an account, or if you need your session keys.

// Network Session Key (MSB)
uint8_t NwkSkey[16] = { 0x03, 0xFE, 0xEA, 0xA4, 0x21, 0x7B, 0x0D, 0xEB, 0x6E, 0x7C, 0x16, 0xDE, 0x56, 0xB5, 0x8E, 0x50 };

// Application Session Key (MSB)
uint8_t AppSkey[16] = { 0x28, 0x60, 0x81, 0x13, 0xE1, 0x0C, 0xAB, 0xB0, 0x90, 0x48, 0xE1, 0x73, 0xF0, 0xE4, 0x18, 0xA4 };

// Device Address (MSB)
uint8_t DevAddr[4] = { 0x26, 0x01, 0x1B, 0xCE };

/************************** Example Begins Here ***********************************/
// Data Packet to Send to TTN
unsigned char loraData[11] = {"hello LoRa"};

// How many times data transfer should occur, in seconds
const unsigned int sendInterval = 30;

// Pinout for Adafruit Feather 32u4 LoRa
TinyLoRa lora = TinyLoRa(7, 8);

// Pinout for Adafruit Feather M0 LoRa
//TinyLoRa lora = TinyLoRa(3, 8);

void setup()
{
  delay(2000);
  //Serial.begin(9600);
  //while (! Serial);
  
  // Initialize pin LED_BUILTIN as an output
  pinMode(LED_BUILTIN, OUTPUT);
  
  // Initialize LoRa
  Serial.print("Starting LoRa...");
  // define multi-channel sending
  lora.setChannel(MULTI);
  // set datarate
  lora.setDatarate(SF7BW125);
  // set TxPower
  lora.setTxPower(23);
  if(!lora.begin())
  {
    //Serial.println("Failed");
    //Serial.println("Check your radio");
    while(true);
  }
  //Serial.println("OK");
}

void loop()
{
  //Serial.println("Sending LoRa Data...");
  lora.sendData(loraData, sizeof(loraData), lora.frameCounter);
  //Serial.print("Frame Counter: ");Serial.println(lora.frameCounter);
  lora.frameCounter++;

  // blink LED to indicate packet sent
  //digitalWrite(LED_BUILTIN, HIGH);
  //delay(100);
  //digitalWrite(LED_BUILTIN, LOW);
  
  //Serial.println("delaying...");
  delay(sendInterval * 100);
}
