// Konstantinos Dimitrakopoulos
// 6/5/2019
// Version 1.01

/*  Application to connect to the_things_network using the Adafruit Feather 32u4 RFM95 LoRa Radio
    and send voltage measurements to the gateway
*/

/************************** Configuration ***********************************/
#include <TinyLoRa.h>
#include <SPI.h>
#include <Adafruit_SleepyDog.h>


#define VBATPIN A7

// Radio Settings

#define NumberMsg 30         /*  How many messages the radio will send before going to sleep
                                0 for Unlimited */

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

#define TxPower 14          // 5..23



// Network Session Key (MSB)
uint8_t NwkSkey[16] = { 0x03, 0xFE, 0xEA, 0xA4, 0x21, 0x7B, 0x0D, 0xEB, 0x6E, 0x7C, 0x16, 0xDE, 0x56, 0xB5, 0x8E, 0x50 };

// Application Session Key (MSB)
uint8_t AppSkey[16] = { 0x28, 0x60, 0x81, 0x13, 0xE1, 0x0C, 0xAB, 0xB0, 0x90, 0x48, 0xE1, 0x73, 0xF0, 0xE4, 0x18, 0xA4 };

// Device Address (MSB)
uint8_t DevAddr[4] = { 0x26, 0x01, 0x1B, 0xCE };


/************************** Code ***********************************/


// Data Packet to Send to TTN
unsigned char loraData[2];

// Pinout for Adafruit Feather 32u4 LoRa
TinyLoRa lora = TinyLoRa(7, 8);

// Voltage Measurement
float measuredvbat;


enum State_enum {INIT, MEASURE_V, SLEEP, SEND, ERROR, STOP};
uint8_t state = INIT;

// States
void init_state();
void measure_v_state();
void send_state();
void sleep_state();
void error_state();
void stop_state();

void setup(){  
}

void loop(){

    switch(state){
      case INIT:
      init_state();
      break;

      case MEASURE_V:
      measure_v_state();
      break;

      case SLEEP:
      sleep_state();
      break;

      case SEND:
      send_state();
      break;

      case ERROR:
      error_state();
      break;
      
      case STOP:
      stop_state();
      break;
    }
}

void init_state(){
  Watchdog.disable();

  pinMode(LED_BUILTIN, OUTPUT);
  
  lora.setChannel(Channel);
  lora.setDatarate(Datarate);
  lora.setTxPower(TxPower);

  if(!lora.begin())
  {
    state = ERROR;
  }
  else
  {
    state = MEASURE_V;
  }
}

void measure_v_state(){

  /*Meassurent happens after the voltage divider (r1,r2=100kohm)
    so we need to muplity by 2,
    then multiply by the reference voltage of the board 
    and divide by 1024 to convert the bits to voltage 
  */
  measuredvbat = analogRead(VBATPIN);
  measuredvbat *= 2;    
  measuredvbat *= 3.15;  
  measuredvbat /= 1024; 

  //If battery is below 3.5 go to sleep and check again in 8 seconds
  if(measuredvbat < 3.5){
    state = SLEEP;
  }
  else{
    state = SEND;
  }
}

void sleep_state(){
  Watchdog.sleep(8000);
  state = MEASURE_V;
}

void send_state(){
  int16_t VInt = round(measuredvbat * 100);
  loraData[0] = highByte(VInt);
  loraData[1] = lowByte(VInt);
  lora.sendData(loraData, sizeof(loraData), lora.frameCounter);
  lora.frameCounter++; 

  if(NumberMsg !=0 && lora.frameCounter > NumberMsg){
    state = STOP;
  }
  else{
  Watchdog.sleep(4000);
  state = MEASURE_V;
  }
}

void stop_state(){
  digitalWrite  (LED_BUILTIN, HIGH);
  Watchdog.sleep(8000);

  state = STOP;
}

void error_state(){
  Watchdog.enable(4000);
  while(true){
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
  }
}
