#include <ROCKET_SEQUENCER.h>

void led_test(){
  digitalWrite(13,HIGH);
}

void setup() {
  // put your setup code here, to run once:
  pinMode(13,OUTPUT);
  digitalWrite(13,LOW);
  initialize(1);
  loadSequence(0,&led_test,_status);
  set_condition(&getTimer,10000,EQUAL,0);
}

void loop() {
  // put your main code here, to run repeatedly:
  start();
}
