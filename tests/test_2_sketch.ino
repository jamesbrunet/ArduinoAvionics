#include <ROCKET_SEQUENCER.h>
#ifndef LED_PIN
#define LED_PIN 13
#endif

/*
void led_test(){
  digitalWrite(13,HIGH);
}
*/

void abort_test(){
  Serial.println(F("ABORT! This is a test"));
  digitalWrite(LED_PIN,HIGH);
}

void launch_print(){
  Serial.println(F("We have ignition!"));
}



void setup() {
  // put your setup code here, to run once:
  pinMode(LED_PIN,OUTPUT);
  digitalWrite(LED_PIN,LOW);
  Serial.begin(9600); // serial connection baud rate 9600
  initialize(3);
  loadSequence(0,&abort_test);
  set_condition(ROCKET_ABORT,EQUAL,0);
  loadSequence(1,&launch_print);
  set_condition(&getTimer,0,EQUAL,1);
  loadSequence(2,ROCKET_ABORT);
  set_condition(&getTimer,10000,EQUAL,2);
}

void loop() {
  // put your main code here, to run repeatedly:
  start();
}
