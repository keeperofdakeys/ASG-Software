/*
    ASG Controller Code
    
    Designed to run on a Freetronics Leostick, but will work on any other Arduino board.
    
    
    Copyright (C) 2013 Mark Jessop <mark.jessop@adelaide.edu.au>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    For a full copy of the GNU General Public License, 
    see <http://www.gnu.org/licenses/>.

*/


#if ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include <TimerOne.h>

// PIN DEFINITIONS
#define FIRE  2
#define AZ_DIR  9
#define AZ_STEP 8
#define LED    13
#define POT_INPUT   5

#define STEP_UNIT  500
#define FIRE_PERIOD  1500

#define TIMER_PERIOD  300
unsigned int fire_timer = 0;
unsigned int step_timer = 0;

void asg_isr(){
   if(fire_timer>0) fire_timer--;
   if(step_timer>0) step_timer--; 
   
   if(fire_timer==0) digitalWrite(FIRE, LOW);
   if(step_timer&1 == 1) digitalWrite(AZ_STEP, HIGH);
   if(step_timer&0 == 0) digitalWrite(AZ_STEP, LOW);
   if(step_timer==0 && fire_timer==0) digitalWrite(LED, LOW);
}

void right(){
  digitalWrite(AZ_DIR, HIGH);
}

void left(){
  digitalWrite(AZ_DIR, LOW);
}

void step(){
  Timer1.detachInterrupt();
  step_timer = step_timer + STEP_UNIT;
  digitalWrite(LED, HIGH);
  Timer1.attachInterrupt(asg_isr);
}

void stop(){
  Timer1.detachInterrupt();
  fire_timer = 0;
  step_timer = 0;
  Timer1.attachInterrupt(asg_isr);
}

void fire(){
  Timer1.detachInterrupt();
  fire_timer = fire_timer + FIRE_PERIOD;
  digitalWrite(FIRE, HIGH);
  digitalWrite(LED, HIGH);
  Timer1.attachInterrupt(asg_isr);
}

void setup(){
  pinMode(FIRE, OUTPUT);
  digitalWrite(FIRE,LOW);
  pinMode(AZ_DIR, OUTPUT);
  pinMode(AZ_STEP, OUTPUT);
  digitalWrite(AZ_DIR, LOW);
  digitalWrite(AZ_DIR, LOW);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  
  Serial.begin(9600);
  
  Timer1.initialize(TIMER_PERIOD);
  Timer1.attachInterrupt(asg_isr);
}

void loop(){
  char incomingByte = 0;
  if( Serial.available()>0 ){
      incomingByte = Serial.read();
      
      if(incomingByte == '<'){
        //Serial.println("OK");
        left(); step();
      }else if(incomingByte == '>'){
        //Serial.println("OK");
        right(); step();
      }else if(incomingByte == 'f'){
        //Serial.println("OK");
         fire();
      }else if(incomingByte == 's'){
        stop();
      }else if(incomingByte == 'p'){
        Serial.println(analogRead(POT_INPUT));
      
      }else{
        //Serial.println("ERROR");
      }
      
  }
  
}
