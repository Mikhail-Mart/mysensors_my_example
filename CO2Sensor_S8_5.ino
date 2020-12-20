/**
 * The MySensors Arduino library handles the wireless radio link and protocol
 * between your home built sensors/actuators and HA controller of choice.
 * The sensors forms a self healing radio network with optional repeaters. Each
 * repeater and gateway builds a routing tables in EEPROM which keeps track of the
 * network topology allowing messages to be routed to nodes.
 *
 * Created by Henrik Ekblad <henrik.ekblad@mysensors.org>
 * Copyright (C) 2013-2015 Sensnology AB
 * Full contributor list: https://github.com/mysensors/Arduino/graphs/contributors
 *
 * Documentation: http://www.mysensors.org
 * Support Forum: http://forum.mysensors.org
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 *******************************
 *
 * DESCRIPTION
 *
 *  S_8_AIR CO2 sensor
 *
 *  Wiring:
 *   Pad 1, Pad 5: Vin (Voltage input 4.5V-6V)
 *   Pad 2, Pad 3, Pad 12: GND
 *   rx/tx 7 8
 *
 *	From: http://davidegironi.blogspot.fr/2014/01/co2-meter-using-ndir-infrared-S_8_AIR.html

 * 	From: http://airqualityegg.wikispaces.com/Sensor+Tests
 *	  - response time is less than 30 s
 *   - 3 minute warm up time
 *	datasheet: http://www.futurlec.com/Datasheet/Sensor/S_8_AIR.pdf
 * Contributor: epierre
 */

#include <SoftwareSerial.h>
SoftwareSerial S8_Serial(7,8);
byte readCO2[] = {0xFE, 0X44, 0X00, 0X08, 0X02, 0X9F, 0X25};  //Command packet to read Co2 (see app note)
byte response[] = {0,0,0,0,0,0,0};  //create an array to store the response

//multiplier for value. default is 1. set to 3 for K-30 3% and 10 for K-33 ICB
int valMultiplier = 1;


// Enable debug prints to serial monitor
#define MY_DEBUG

// Enable and select radio type attached
#define MY_RADIO_NRF24
//#define MY_RADIO_NRF5_ESB
//#define MY_RADIO_RFM69
//#define MY_RADIO_RFM95

#include <MySensors.h>

#define CHILD_ID_AIQ 0
//#define AIQ_SENSOR_ANALOG_PIN 6

uint32_t SLEEP_TIME = 30*1000; // Sleep time between reads (in milliseconds)
//uint32_t SLEEP_TIME = 120*1000; // Sleep time between reads (in milliseconds) после этой чтрочки модуль не просыпается



float valAIQ =0.0;
float lastAIQ =0.0;

MyMessage msg(CHILD_ID_AIQ, V_LEVEL);
MyMessage msg2(CHILD_ID_AIQ, V_UNIT_PREFIX);

void setup()
{
  S8_Serial.begin(9600);    //Opens the virtual serial port with a baud of 9600 
	
}

void presentation()
{
	// Send the sketch version information to the gateway and Controller
	sendSketchInfo("AIQ Sensor CO2 S8_AIR", "1.0");

	// Register all sensors to gateway (they will be created as child devices)
	present(CHILD_ID_AIQ, S_AIR_QUALITY);
	send(msg2.set("ppm"));
}

void loop()
{


  sendRequest(readCO2);
  unsigned long co2ppm = getValue(response);



  Serial.print("Co2 ppm = ");
	Serial.print(co2ppm);
  Serial.print(" last ");
 Serial.print(lastAIQ);
  Serial.println();
 
	if ((co2ppm != lastAIQ)&&(abs(co2ppm-lastAIQ)>=10)) {
		send(msg.set((unsigned long)ceil(co2ppm)));
  // send(msg.set(ceil(co2ppm)));
    lastAIQ = ceil(co2ppm);
	}

	

	// Power down the radio.  Note that the radio will get powered back up
	// on the next write() call.
	
	sleep(SLEEP_TIME); //sleep for: sleepTime

 
}


void sendRequest(byte packet[])
{
  while(!S8_Serial.available())  //keep sending request until we start to get a response
  {
    S8_Serial.write(readCO2,7);
    delay(50);
  }
  
  int timeout=0;  //set a timeoute counter
  while(S8_Serial.available() < 7 ) //Wait to get a 7 byte response
  {
    timeout++;  
    if(timeout > 10)    //if it takes to long there was probably an error
      {
        while(S8_Serial.available())  //flush whatever we have
          S8_Serial.read();
          
          break;                        //exit and try again
      }
      delay(50);
  }
  
  for (int i=0; i < 7; i++)
  {
    response[i] = S8_Serial.read();
  }  
}

unsigned long getValue(byte packet[])
{
    int high = packet[3];                        //high byte for value is 4th byte in packet in the packet
    int low = packet[4];                         //low byte for value is 5th byte in the packet

  
    unsigned long val = high*256 + low;                //Combine high byte and low byte with this formula to get value
    return val* valMultiplier;
}
