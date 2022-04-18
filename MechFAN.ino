#include <Arduino.h>
#include <TimerOne.h>
#include "cc1101.h"

#define REQUEST_RATE    2500 // ms
#define ONBOARD_LED 7
#define PAIR_TIME_OUT   10000 // ms
#define RX_TIME_OUT     300    // ms
#define LED_BLINK_SPEED 250   // ms

enum module_states
{
  JUST_BOOTED,
  RF15_PAIRINGSMODE,
  NORMAL_MODE  
};

CC1101 radio;

volatile bool request_state_flag = 0;

//******************************************************************************************//
//                                                                                          //
//                        1ms timer                                                         //
//                                                                                          //
//******************************************************************************************//
void timerIsr()
{ 
  static int led_blink_cntr = 0;
  static int request_rate_cntr = 0;

  // ############### BEGIN REQUEST RATE COUNTER ###############
  if(request_rate_cntr < REQUEST_RATE)
    request_rate_cntr++;
  else 
  {
    request_state_flag = true;
    request_rate_cntr = 0;
  }
  // ############### END REQUEST RATE COUNTER ###############  
  
  // ############### BEGIN LED BLINK COUNTER ###############
  if(led_blink_cntr < LED_BLINK_SPEED)
    led_blink_cntr++;
  else 
  {
    digitalWrite(ONBOARD_LED, !digitalRead(ONBOARD_LED)); // Toggle LED
    led_blink_cntr = 0;
  }
  // ############### END LED BLINK COUNTER ###############  

  // ############### BEGIN TIMEOUT FOR PAIRING ###############
  if(radio.pair_timeout_cntr < PAIR_TIME_OUT)
      radio.pair_timeout_cntr++;      
  if(radio.pair_timeout_cntr == PAIR_TIME_OUT)
  {
    radio.pair_timeout_flag = true;
    radio.pair_timeout_cntr++;
  }
  // ############### END TIMEOUT FOR PAIRING ###############

  // ############### BEGIN TIMEOUT FOR RX ###############
  if(radio.rx_timeout_cntr < RX_TIME_OUT)
      radio.rx_timeout_cntr++;      
  if(radio.rx_timeout_cntr == RX_TIME_OUT)
  {
    radio.rx_timeout_flag = true;
    radio.rx_timeout_cntr++;
  }
  // ############### END TIMEOUT FOR PAIRING ###############
  
}

//******************************************************************************************//
//                                                                                          //
//                        Init                                                              //
//                                                                                          //
//******************************************************************************************//
void setup() 
{       
    radio.debug_flag = true; // Debug messages via terminal (USB Serial)
    
    pinMode(ONBOARD_LED, OUTPUT);
    digitalWrite(ONBOARD_LED, HIGH);
    
    if(radio.debug_flag) 
      Serial.begin(9600);   // Virtual comport via USB
      
    Serial1.begin(38400); // used for transmitting data to Orcon  
    delay(2500);

    Timer1.initialize(1000); // us
    Timer1.attachInterrupt(timerIsr); // attach the service routine here

    // Init vars
    radio.pair_timeout_cntr = PAIR_TIME_OUT+1;
    radio.rx_timeout_cntr = RX_TIME_OUT+1;
        
}

//******************************************************************************************//
//                                                                                          //
//                        MAIN                                                              //
//                                                                                          //
//******************************************************************************************//
void loop() 
{

  module_states dongle_state = JUST_BOOTED; 
  
  if(radio.debug_flag)
    delay(2500);
  
  while(1)
  {
    switch(dongle_state)
    {
      case(JUST_BOOTED):
        radio.init();
        
        if((radio.readReg(CC1101_MARCSTATE, CC1101_STATUS_REGISTER) & 0x1f) == 1)    
        {
          if(radio.debug_flag) Serial.println("> CC1101 radio initialized");
          dongle_state = RF15_PAIRINGSMODE;
        }
        else
        {
          if(radio.debug_flag) Serial.println("> Something went wrong with radio init, will try again");
          delay(1000);
        }
        break;
        
        case(RF15_PAIRINGSMODE):
        
          if(radio.debug_flag) 
          {
            Serial.println("> Enter cloning mode ");
            Serial.print("> Press button on RF15 [timeout after ");
            Serial.print((int)(PAIR_TIME_OUT*0.001));
            Serial.println("s]");
          }      
          radio.set_rx_mode();          

          if(radio.clone_mode())                // try to clone system, timeouts after 5sec 
          {
            if(radio.debug_flag) 
              Serial.println("> Clone succesfull!\n");
          }
          else
          {
            if(radio.debug_flag) 
              Serial.println("> No RF15 messages received!\n");
          }
          /* EEPROM ADDRESS
           *  0 - 2 => RF15 address
           *  3 - 5 => ORCON broadcast address
           */
          if(radio.debug_flag) 
          {
            Serial.print("> Using source device id: ");
            for(int i = 0; i < 3; i++)
            {
              Serial.print(EEPROM.read(i), HEX);
              Serial.print(" ");
            }
            Serial.println("");
            Serial.print("> Using target device id: ");
            for(int i = 3; i < 6; i++)
            {
              Serial.print(EEPROM.read(i), HEX);
              Serial.print(" ");
            }          
            Serial.println("");
            Serial.println("");
            Serial.println("> Enter idle mode");
          }

         
          dongle_state = NORMAL_MODE;          
          
          break;

          case(NORMAL_MODE):
            
            if(request_state_flag)
            {
              Serial.println(radio.request_orcon_state());
              request_state_flag = false;
            }
            
            break;          
    }

  } 
 
}
