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
 *  MH_Z19_AIR CO2 sensor
 *
 *  Wiring:
 *   Pad 1, Pad 5: Vin (Voltage input 4.5V-5V)
 *   Pad 2, Pad 3, Pad 12: GND
 *   rx/tx 7 8
 *
 * Contributor: epierre
 */

#include <SoftwareSerial.h>
SoftwareSerial z19_Serial(7,8);
byte cmd[9] = {0xFF,0x01,0x86,0x00,0x00,0x00,0x00,0x00,0x79}; 
unsigned char response[9];



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
  z19_Serial.begin(9600);    //Opens the virtual serial port with a baud of 9600 
	
}

void presentation()
{
	// Send the sketch version information to the gateway and Controller
	sendSketchInfo("AIQ Sensor CO2 MH_Z19_AIR", "1.0");

	// Register all sensors to gateway (they will be created as child devices)
	present(CHILD_ID_AIQ, S_AIR_QUALITY);
	send(msg2.set("ppm"));
}

void loop()
{



  z19_Serial.write(cmd, 9);
  //memset(response, 0, 9);
  z19_Serial.readBytes(response, 9);
  int i;
  byte crc = 0;
  for (i = 1; i < 8; i++) crc+=response[i];
  crc = 255 - crc;
  crc++;

 


  if ( !(response[0] == 0xFF && response[1] == 0x86 && response[8] == crc) ) {
    
    Serial.println("CRC error: " + String(crc) + " / "+ String(response[8]));
    
  } else {
    
    unsigned int responseHigh = (unsigned int) response[2];
    unsigned int responseLow = (unsigned int) response[3];
    unsigned int co2ppm = (256*responseHigh) + responseLow;

  if ((co2ppm != lastAIQ)&&(abs(co2ppm-lastAIQ)>=10)) {
    send(msg.set((unsigned int)ceil(co2ppm)));
  // send(msg.set(ceil(co2ppm)));
    lastAIQ = ceil(co2ppm);
  Serial.println(co2ppm);
  }

    
    
  }


 


	

	// Power down the radio.  Note that the radio will get powered back up
	// on the next write() call.
	
	sleep(SLEEP_TIME); //sleep for: sleepTime

 
}
