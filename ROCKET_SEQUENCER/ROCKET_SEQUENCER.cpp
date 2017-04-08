/*
 * CUInSpace Rocket Sequencer v0.2 (attempts at decrapifying and ditching class)
 * It defines the basic framework for the programs that run on arduino-based rocket avionics system. 
 * It simulates what would be called a sequencer on a real rocket, providing time frame and 
 * starting certain actions based on a certain trigger (be it time-based or otherwise) and 
 * oversees programs' executions.
----------------------------------------
 * This is still in preliminary stage. Anyone is welcome to improve it!
 */
 

#include "Arduino.h"
#include "ROCKET_SEQUENCER.h"
#undef NULL
#define NULL 0

// variable definitions
_event_t* sequence;
unsigned char events;
status_t _status;
unsigned long timer;
unsigned long trigger_zero;//record trigger start time (in respect to program's start time)
unsigned short countdown_time;
int triggerPin;

//*******************************
// GET the status of the rocket
status_t getStatus() {
	return _status;
}
//*******************************

//*******************************
// set the status of the rocket (probably redundant, but whatever)

void setStatus(status_t status) {
	_status = status; // set the rocket status to trigger some event triggered by statuses
}


//*******************************

//*******************************
//Initializer, set trigger pin and number of events

void initialize(unsigned char stage_num, int trigger_pin) {
	events = stage_num;
	sequence = new _event_t[stage_num];
	setStatus(ROCKET_GROUND); // NO MISFIRE HAPPENING
	timer = 0;
	countdown_time = 0;
	triggerPin = trigger_pin;
	if (trigger_pin != -1) pinMode(trigger_pin, INPUT); // if trigger pin is set, set that pin as input
}

//*******************************

//*******************************
//Set countdown time(in milliseconds)
void setCountdownTime(unsigned short time) {
	countdown_time = time;
}
//*******************************



//*******************************
// several overloads to associate a function (returns nothing and takes
// one/two arguments of unsigned short type/nothing at all) with the event or
// just to set the rocket status

void loadSequence(unsigned stage_num, function_2arg_t action, unsigned short arg1, unsigned short arg2,
			  status_t status_to_trigger ) { // load a function onto the sequencer (with two arguments)
	
	sequence[stage_num].argc = 2;
	sequence[stage_num].function_twoarg = action;
	sequence[stage_num].arg1 = arg1;
	sequence[stage_num].arg2 = arg2;
	sequence[stage_num].status_to_trigger = status_to_trigger;
}

void loadSequence(unsigned stage_num, function_onearg_t action, unsigned short arg1,
			  status_t status_to_trigger ){ //load a function with one argument
	sequence[stage_num].argc = 1;
	sequence[stage_num].function_onearg = action;
	sequence[stage_num].arg1 = arg1;
	sequence[stage_num].status_to_trigger = status_to_trigger;
}

void loadSequence(unsigned stage_num, function_noarg_t action,
			  status_t status_to_trigger ){ //load a function with no argument
	sequence[stage_num].argc = 0;
	sequence[stage_num].function_noarg = action;
	sequence[stage_num].status_to_trigger = status_to_trigger;
}

void loadSequence(unsigned stage_num,
			  status_t status_to_trigger ) {//just setting a flag
	sequence[stage_num].argc = 0;
	sequence[stage_num].function_noarg = NULL;
	sequence[stage_num].status_to_trigger = status_to_trigger;
}

//*******************************

//*******************************
// the function defines the condition that the function executes
// using unsigned shorts or statuses (sorry, only one condition can be put in)

void set_condition(func_compare_t cmp1, unsigned long cmp2,
			   compare_t sign_input, unsigned stage_num) {
	sequence[stage_num].func_to_compare = cmp1;
	sequence[stage_num].cmp2 = cmp2;
	sequence[stage_num].sign = sign_input;
}

void set_condition(status_t status_to_compare,
		compare_t sign_input, unsigned stage_num) {
	sequence[stage_num].status = status_to_compare;
	sequence[stage_num].sign = sign_input;
}
//*******************************

//*******************************
//executes the action associated in each event
void exec_event(_event_t event_name) { // execute the function predefined in loadSequence()
					       // except when function pointer is null
	switch(event_name.argc) {
		case 0:
		if (event_name.function_noarg != NULL) {(*(event_name.function_noarg)) ();}
		break;
		
		case 1:
		if (event_name.function_onearg != NULL) {(*(event_name.function_onearg)) (event_name.arg1);}
		break;
		
		case 2:
		if (event_name.function_twoarg != NULL) {(*(event_name.function_twoarg)) (event_name.arg1, event_name.arg2);}
		break;
		
		default:
		setStatus(ROCKET_ABORT); // set the abort flag if more than two arguments
		break;
	}
	setStatus(event_name.status_to_trigger);
}
//*******************************

//*******************************
// the only statement to be put in loop(). other statements in the library goes to setup()
// provides countdown and only executes crucial commands in the event of ROCKET_ABORT

void start() {
	/* countdown and trigger portion */
	if (getStatus() == ROCKET_GROUND) {
		if (triggerPin != -1) {
			if (digitalRead(triggerPin) == HIGH)
			setStatus(ROCKET_COUNTDOWN);
			else trigger_zero = millis();
		}
		else setStatus(ROCKET_COUNTDOWN);
		return;
	}
	else if (getStatus() == ROCKET_COUNTDOWN) {
		if (millis() - trigger_zero >= countdown_time) setStatus(STAGE_START);
		else delay(1);
		return;
	}
	
	timer = millis() - countdown_time - trigger_zero;
	for (unsigned char i = 0; i < events && //when rocket is not grounded/armed/in countdown
		!(getStatus() == ROCKET_GROUND || getStatus() == ROCKET_COUNTDOWN); i++) {
		switch (sequence[i].sign) {
			case AT:
			if ((*(sequence[i].func_to_compare)) () == sequence[i].cmp2) {
				if(getStatus() != ROCKET_ABORT) exec_event(sequence[i]);
			}
			if (_status == sequence[i].status) {
				exec_event(sequence[i]);
			}
			break;
			
			case FROM:
			if ((*(sequence[i].func_to_compare)) () > sequence[i].cmp2 || 
			    _status > sequence[i].status) {
				if(getStatus() != ROCKET_ABORT) exec_event(sequence[i]);
			}
			break;
			
			case UNTIL:
			if ((*(sequence[i].func_to_compare)) () < sequence[i].cmp2 || 
			    _status < sequence[i].status) {
				if(getStatus() != ROCKET_ABORT) exec_event(sequence[i]);
			}
			
		}
		
	}
	
	
	delay(1);
}

unsigned long getTimer() {
  return timer;
}
//*******************************
