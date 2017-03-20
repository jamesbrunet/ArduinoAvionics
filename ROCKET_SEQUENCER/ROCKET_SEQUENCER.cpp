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
_event_t* sequence = NULL;
unsigned char events = 0;
status_t _status = ROCKET_GROUND;
short timer = 0;

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
}

//*******************************



//status_t Rocket::getStatus() {return this->_status;} // get the rocket's status

//*******************************
// several overloads to associate a function (returns nothing and takes
// one/two arguments of unsigned short type/nothing at all) with the event or
// just to set the rocket status

void loadSequence(_event_t* event_name, function_2arg_t action, unsigned short arg1, unsigned short arg2,
			  status_t status_to_trigger ) { // load a function onto the sequencer (with two arguments)
	
	event_name->argc = 2;
	event_name->function_twoarg = action;
	event_name->arg1 = arg1;
	event_name->arg2 = arg2;
	setStatus(status_to_trigger);
}

void loadSequence(_event_t* event_name, function_onearg_t action, unsigned short arg1,
			  status_t status_to_trigger ){ //load a function with one argument
	event_name->argc = 1;
	event_name->function_onearg = action;
	event_name->arg1 = arg1;
	setStatus(status_to_trigger);
}

void loadSequence(_event_t* event_name, function_noarg_t action,
			  status_t status_to_trigger ){ //load a function with no argument
	event_name->argc = 0;
	event_name->function_noarg = action;
	setStatus(status_to_trigger);
}

void loadSequence(_event_t* event_name,
			  status_t status_to_trigger ) {//just setting a flag
	event_name->argc = 0;
	event_name->function_noarg = NULL;
	setStatus(status_to_trigger);
}

//*******************************

//*******************************
// the function defines the condition that the function executes
// using unsigned shorts or statuses (sorry, only one condition can be put in)

void set_condition(func_compare_t cmp1, unsigned short cmp2,
			   compare_t sign_input, _event_t* event_name) {
	event_name->func_to_compare = cmp1;
	event_name->cmp2 = cmp2;
	event_name->sign = sign_input;
}

void set_condition(status_t rocket_status, status_t status_to_compare,
		compare_t sign_input, _event_t* event_name) {
	event_name->current_status = rocket_status;
	event_name->status = status_to_compare;
	event_name->sign = sign_input;
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

void run() {
	timer = millis();
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
	
	for (unsigned char i = 0; i < events || getStatus() == ROCKET_ABORT; i++) {
		switch (sequence[i].sign) {
			case EQUAL:
			if ((*(sequence[i].func_to_compare)) () == sequence[i].cmp2) {
				if(getStatus() != ROCKET_ABORT) exec_event(sequence[i]);
			}
			if (sequence[i].current_status == sequence[i].status) {
				exec_event(sequence[i]);
			}
			break;
			
			case GREATER_THAN:
			if ((*(sequence[i].func_to_compare)) () > sequence[i].cmp2 || 
			    sequence[i].current_status > sequence[i].status) {
				if(getStatus() != ROCKET_ABORT) exec_event(sequence[i]);
			}
			break;
			
			case SMALLER_THAN:
			if ((*(sequence[i].func_to_compare)) () < sequence[i].cmp2 || 
			    sequence[i].current_status < sequence[i].status) {
				if(getStatus() != ROCKET_ABORT) exec_event(sequence[i]);
			}
			break;
			
			case GREATER_THAN_EQUAL_TO:
			if ((*(sequence[i].func_to_compare)) () >= sequence[i].cmp2 || 
			    sequence[i].current_status >= sequence[i].status) {
				if(getStatus() != ROCKET_ABORT) exec_event(sequence[i]);
			}
			break;
			
			case SMALLER_THAN_EQUAL_TO:
			if ((*(sequence[i].func_to_compare)) () <= sequence[i].cmp2 || 
			    sequence[i].current_status <= sequence[i].status) {
				if(getStatus() != ROCKET_ABORT) exec_event(sequence[i]);
			}
			break;
			
			case NOT_EQUAL:
			if ((*(sequence[i].func_to_compare)) () != sequence[i].cmp2 || 
			    sequence[i].current_status != sequence[i].status) {
				if(getStatus() != ROCKET_ABORT) exec_event(sequence[i]);
			}
			break;
		}
		
	}
	
	if (getStatus() != ROCKET_ABORT) {
		delay(4);
	}
}

short getTimer() {
  return timer;
}
//*******************************
