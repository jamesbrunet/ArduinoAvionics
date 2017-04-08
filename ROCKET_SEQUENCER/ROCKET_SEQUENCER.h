/*
 * CUInSpace Rocket Sequencer v0.2 (attempts at decrapifying and ditching class)
 * It defines the basic framework for the programs that run on arduino-based rocket avionics system. 
 * It simulates what would be called a sequencer on a real rocket, providing time frame and 
 * starting certain actions based on a certain trigger (be it time-based or otherwise) and 
 * oversees programs' executions.
----------------------------------------
 * This is still in preliminary stage. Anyone is welcome to improve it!
 */

#ifndef ROCKET_SEQUENCER_H
#define ROCKET_SEQUENCER_H

// this is a test comment
typedef unsigned long (*func_compare_t) (void);
typedef void (*function_noarg_t) (void); 		
typedef void (*function_onearg_t) (unsigned short);
typedef void (*function_2arg_t) (unsigned short, unsigned short);

enum status_t {ROCKET_GROUND, //rocket is safed
	      ROCKET_COUNTDOWN, //rocket is in countdown (I assume the arming will be done from the box)
	      STAGE_ARMED,  //air-start ignition circuit moved from safe to armed
	      STAGE_START, //air-start/first-stage ignition started
	      PAYLOAD_START, // payload release mechanism starts
	      ROCKET_ABORT // the launch is aborted
}; 

enum compare_t {AT, FROM, UNTIL};



typedef struct { // pointers to functions to call before and up to rocket's each stage startup and in each stage's duration
	union{function_noarg_t function_noarg;
		function_onearg_t function_onearg;
		function_2arg_t function_twoarg;};
	func_compare_t func_to_compare;
	union{unsigned long cmp2; status_t status;};
	unsigned char arg1;
	unsigned char arg2;
	compare_t sign;
	status_t status_to_trigger;
	unsigned char argc;
} _event_t;

extern _event_t* sequence;
extern unsigned char events;
extern status_t _status;
extern unsigned long trigger_zero;//record rocket start time (in respect to program's start time)
extern unsigned short countdown_time;//countdown time (in milliseconds)
extern int triggerPin;
extern unsigned long timer; // the rocket sequencer timer in milliseconds (both count down and count up)

extern void initialize(unsigned char stages, int trigger_pin = -1); //define rocket stages and trigger pin
extern void setCountdownTime(unsigned short time); // set countdown time(in milliseconds)
extern void loadSequence(unsigned stage_num, function_2arg_t action, unsigned short arg1, unsigned short arg2,
		status_t status_to_trigger = _status); // load a function onto the sequencer
extern void loadSequence(unsigned stage_num, function_onearg_t action, unsigned short arg1,
		status_t status_to_trigger = _status); //load a function with one argument
extern void loadSequence(unsigned stage_num, function_noarg_t action,
		status_t status_to_trigger = _status); //load a function with no argument
extern void loadSequence(unsigned stage_num, 
		status_t status_to_trigger = _status); /*
			  		      * also, a neat feature: when combined with 
			  		      * set_condition() and no function 
			  		      * is defined, it will just set flag when a 
			  		      * condition is met (e.g. a stop signal is sent during 
			  		      * countdown due to a plane from homeland security fouling
			  		      * the range)
			  		      */
extern void set_condition(func_compare_t cmp1, unsigned long cmp2,
		compare_t sign_input, unsigned stage_num); // set the event's starting condition
extern void set_condition(status_t status_to_compare,
		compare_t sign_input, unsigned stage_num); //function overload for statuses
extern void start(); // start the sequencer (THE FLIGHT COMPUTER HAS TAKEN OVER THE COUNTDOWN)
extern status_t getStatus(); // GET the status of the rocket (as defined in enum status_t)
extern void setStatus(status_t status); // set the status of the rocket
extern unsigned long getTimer(); // get the time in milliseconds from the rocket timer



#endif // ROCKET_SEQUENCER_H
