// Kdi-ttn
/************************** Configuration ***********************************/
#include <TinyLoRa.h>
#include <SPI.h>
#include <Adafruit_SleepyDog.h>

#define VBATPIN A7

// Radio Settings
#define Datarate SF7BW125   /*  SF7BW125
                                SF7BW250 
                                SF8BW125
                                SF9BW125
                                SF10BW125
                                SF11BW125
                                SF12BW125 */

#define Channel MULTI       /*  CH0
                                CH1
                                CH2
                                CH3
                                CH4
                                CH5
                                CH6
                                CH7
                                MULTI */

#define TxPower 14          // 5..20



// Network Session Key (MSB)
uint8_t NwkSkey[16] = { 0x03, 0xFE, 0xEA, 0xA4, 0x21, 0x7B, 0x0D, 0xEB, 0x6E, 0x7C, 0x16, 0xDE, 0x56, 0xB5, 0x8E, 0x50 };

// Application Session Key (MSB)
uint8_t AppSkey[16] = { 0x28, 0x60, 0x81, 0x13, 0xE1, 0x0C, 0xAB, 0xB0, 0x90, 0x48, 0xE1, 0x73, 0xF0, 0xE4, 0x18, 0xA4 };

// Device Address (MSB)
uint8_t DevAddr[4] = { 0x26, 0x01, 0x1B, 0xCE };

// Data Packet to Send to TTN
//unsigned char loraData[11] = {"hello LoRa"};
unsigned char loraData[2];

// How many times data transfer should occur, in seconds
const unsigned int sendInterval = 30;

// Pinout for Adafruit Feather 32u4 LoRa
TinyLoRa lora = TinyLoRa(7, 8);


void setup()
{
  //delay(2000);
  //Serial.begin(9600);
   
  // Initialize pin LED_BUILTIN as an output
  pinMode(LED_BUILTIN, OUTPUT);
  
  // Initialize LoRa
  // define multi-channel sending
  lora.setChannel(Channel);
  // set datarate
  lora.setDatarate(Datarate);
  // set TxPower
  lora.setTxPower(TxPower);

  if(!lora.begin())
  {
    digitalWrite(LED_BUILTIN, HIGH);
    while(true);
  }

}

void loop()
{

  float measuredvbat = analogRead(VBATPIN);
  measuredvbat *= 2;    // we divided by 2, so multiply back
  measuredvbat *= 3.15;  // Multiply by 3.3V, our reference voltage
  measuredvbat /= 1024; // convert to voltage

   //If battery is below 3.5 go to sleep and check again in 8 seconds
  
  while(measuredvbat < 3.5){
   Watchdog.sleep(8000);
   measuredvbat = analogRead(VBATPIN);
   measuredvbat *= 2;    // we divided by 2, so multiply back
  measuredvbat *= 3.15;  // Multiply by 3.3V, our reference voltage
  measuredvbat /= 1024; // convert to voltage
 }
  int16_t VInt = round(measuredvbat * 100);
  loraData[0] = highByte(VInt);
  loraData[1] = lowByte(VInt);
  
  lora.sendData(loraData, sizeof(loraData), lora.frameCounter);
  lora.frameCounter++;
  Watchdog.sleep(2000);
  //delay(2000);
  //Serial.println("delaying...");
  //delay(sendInterval * 100);

  //Serial.print("VBat: " ); Serial.println(measuredvbat);

}
