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
#define TIMER_ERROR 10 // set timer tolerance to 10ms

// variable definitions
_event_t* sequence;
unsigned char events;
status_t _status;
short timer;

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
//Initializer, set countdown time and number of events

void initialize(unsigned char stage_num) {
	events = stage_num;
	sequence = new _event_t[stage_num];
	setStatus(ROCKET_GROUND); // NO MISFIRE HAPPENING
	timer = 0;
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
	setStatus(status_to_trigger);
}

void loadSequence(unsigned stage_num, function_onearg_t action, unsigned short arg1,
			  status_t status_to_trigger ){ //load a function with one argument
	sequence[stage_num].argc = 1;
	sequence[stage_num].function_onearg = action;
	sequence[stage_num].arg1 = arg1;
	setStatus(status_to_trigger);
}

void loadSequence(unsigned stage_num, function_noarg_t action,
			  status_t status_to_trigger ){ //load a function with no argument
	sequence[stage_num].argc = 0;
	sequence[stage_num].function_noarg = action;
	setStatus(status_to_trigger);
}

void loadSequence(unsigned stage_num,
			  status_t status_to_trigger ) {//just setting a flag
	sequence[stage_num].argc = 0;
	sequence[stage_num].function_noarg = NULL;
	setStatus(status_to_trigger);
}

//*******************************

//*******************************
// the function defines the condition that the function executes
// using unsigned shorts or statuses (sorry, only one condition can be put in)

void set_condition(func_compare_t cmp1, short cmp2,
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

}
//*******************************

//*******************************
// the only statement to be put in loop(). other statements in the library goes to setup()
// provides countdown and only executes crucial commands in the event of ROCKET_ABORT

void start() {
	timer = millis();
	//future implementation (hopefully)
/*
	if (getStatus() == ROCKET_GROUND) {
		delay(1000);
		return;
	}
	else if (getStatus() == ROCKET_COUNTDOWN) {
		//timer -= 1;
		if (timer == 0) setStatus(STAGE_START);
		else {delay(1000); return;}
	}
	*/
	
	for (unsigned char i = 0; i < events /*|| getStatus() == ROCKET_ABORT*/; i++) {
		switch (sequence[i].sign) {
			case EQUAL:
			if (abs((*(sequence[i].func_to_compare)) () - sequence[i].cmp2) <= TIMER_ERROR) {
				if(getStatus() != ROCKET_ABORT) exec_event(sequence[i]);
			}
			if (_status == sequence[i].status) {
				exec_event(sequence[i]);
			}
			break;
			
			case GREATER_THAN:
			if ((*(sequence[i].func_to_compare)) () > (sequence[i].cmp2 + TIMER_ERROR) || 
			    _status > sequence[i].status) {
				if(getStatus() != ROCKET_ABORT) exec_event(sequence[i]);
			}
			break;
			
			case SMALLER_THAN:
			if ((*(sequence[i].func_to_compare)) () < (sequence[i].cmp2 - TIMER_ERROR) || 
			    _status < sequence[i].status) {
				if(getStatus() != ROCKET_ABORT) exec_event(sequence[i]);
			}
			break;
			
			case GREATER_THAN_EQUAL_TO:
			if ((*(sequence[i].func_to_compare)) () >= (sequence[i].cmp2 + TIMER_ERROR) || 
			    _status >= sequence[i].status) {
				if(getStatus() != ROCKET_ABORT) exec_event(sequence[i]);
			}
			break;
			
			case SMALLER_THAN_EQUAL_TO:
			if ((*(sequence[i].func_to_compare)) () <= (sequence[i].cmp2 - TIMER_ERROR) || 
			    _status <= sequence[i].status) {
				if(getStatus() != ROCKET_ABORT) exec_event(sequence[i]);
			}
			break;
			
			case NOT_EQUAL:
			if (abs((*(sequence[i].func_to_compare)) () - sequence[i].cmp2) > TIMER_ERROR || 
			    _status != sequence[i].status) {
				if(getStatus() != ROCKET_ABORT) exec_event(sequence[i]);
			}
			break;
		}
		
	}

	/*
	if (getStatus() != ROCKET_ABORT) {
		delay(1);
	}
	*/
	delay(1);
}

short getTimer() {
  return timer;
}
//*******************************
