#include <ROCKET_SEQUENCER.h>

void led_test (){
  digitalWrite(13, HIGH);
}

void setup(){
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  loadSequence(&sequence[0], &led_test, getStatus());
  set_condition(&getTimer, 10000, EQUAL, &sequence[0]);
}

void loop(){
  run();
}
