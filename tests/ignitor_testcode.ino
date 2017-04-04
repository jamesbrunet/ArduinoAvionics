#include <ROCKET_SEQUENCER.h>

const int SWITCH_PIN = 15; //the pin that starts the timer
const int TIME = 0; // countdown time in seconds
const int FIRE_DURATION = 1000; /*the time duration in which 
                                 *the fire pin is set on HIGH
                                 *(in milliseconds)
                                 */
const int FIRE_PIN = 17;// the pin that sends the firing signal to the relay
// All numbers used to initialize those variables are arbitary.

void fireStart(){
  digitalWrite(FIRE_PIN,HIGH);
  Serial.println("Relay circuit activated");
}

void fireStop(){
  digitalWrite(FIRE_PIN,LOW);
  Serial.println("Relay circuit deactivated");
}

void setup(){
  Serial.begin(9600);
  initialize(2,SWITCH_PIN);
  pinMode(FIRE_PIN,OUTPUT);
  setCountdownTime(0);
  loadSequence(0,&fireStart);
  set_condition(&getTimer,TIME * 1000,AT,0);
  loadSequence(1,&fireStop);
  set_condition(&getTimer,(TIME*1000+FIRE_DURATION),AT,1);
}

void loop(){
  start();
}
