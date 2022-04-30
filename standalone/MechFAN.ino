#include <Arduino.h>
#include "cc1101.h"
#define REQUEST_RATE 2500  // ms
#define ONBOARD_LED 7

enum module_states
{
  JUST_BOOTED,
  RF15_PAIRINGSMODE,
  NORMAL_MODE
};

CC1101 radio;

//******************************************************************************************//
//                                                                                          //
//                        Init                                                              //
//                                                                                          //
//******************************************************************************************//
void setup()
{
  pinMode(ONBOARD_LED, OUTPUT);
  digitalWrite(ONBOARD_LED, HIGH);

  if (radio.debug_flag)
    Serial.begin(9600); // Virtual comport via USB

  Serial1.begin(38400); // used for transmitting data to Orcon  
  delay(2500);

}

//******************************************************************************************//
//                                                                                          //
//                        MAIN                                                              //
//                                                                                          //
//******************************************************************************************//
void loop()
{
  module_states dongle_state = JUST_BOOTED;

  delay(2500);

  while (1)
  {
    switch (dongle_state)
    {
      case (JUST_BOOTED):
        radio.init();

        if ((radio.readReg(CC1101_MARCSTATE, CC1101_STATUS_REGISTER) &0x1f) == 1)
        {
          Serial.println("> CC1101 radio initialized");
          dongle_state = RF15_PAIRINGSMODE;
        }
        else
        {
          Serial.println("> Something went wrong with radio init, will try again");
          delay(1000);
        }

        break;

      case (RF15_PAIRINGSMODE):

        Serial.println("> Enter cloning mode ");
        Serial.println("> Press button on RF15");
        radio.set_rx_mode();

        if (radio.clone_mode()) // try to clone system, timeouts after 5sec 
          Serial.println("> Clone succesfull!\n");
        else
          Serial.println("> No RF15 messages received!\n");

        /*EEPROM ADDRESS
         *0 - 2 => RF15 address
         *3 - 5 => ORCON broadcast address
         */
        Serial.print("> Using source device id: ");
        for (int i = 0; i < 3; i++)
        {
          Serial.print(EEPROM.read(i), HEX);
          Serial.print(" ");
        }

        Serial.println("");
        Serial.print("> Using target device id: ");
        for (int i = 3; i < 6; i++)
        {
          Serial.print(EEPROM.read(i), HEX);
          Serial.print(" ");
        }

        Serial.println("");
        Serial.println("");
        Serial.println("> Enter idle mode");

        dongle_state = NORMAL_MODE;

        break;

      case (NORMAL_MODE):

        Serial.println(radio.request_orcon_state());
        delay(REQUEST_RATE);

        break;
    }
  }
}
