/*
 * FormData.cpp
 *
 *  Created on: Oct 29, 2009
 *      Author: Nancy Minderman
 *      Edited by Rajan Jassal 2014
 */

#include "formdata.h"
#include <stdlib.h>
#include <string.h>
#include <ucos.h>
#include <stdio.h>
#include <ctype.h>

#define MAX_ROTATIONS 10000
#define MAX_RPM 200
#define MIN_INPUT 1
#define FOWARD 1
#define BACKWARDS -1

#define ECE315_ETPU_SM_FULL_STEP_MODE 0
#define ECE315_ETPU_SM_HALF_STEP_MODE 1

/* Name: FormData Constructor
 * Description: Empty Constructor for the class
 * Inputs: 	none
 * Outputs: none
 */
FormData::FormData() {
	// TODO Auto-generated constructor stub

}

/* Name: FormData Destructor
 * Description: Empty Destructor for the class
 * Inputs:	none
 * Outputs: none
 */
FormData::~FormData() {
	// TODO Auto-generated destructor stub
}

/* Name:
 * Description:
 * Inputs:
 * Outputs:
 */
BYTE FormData::SetMaxRPM(char *maxRPM) {
	if(numericCheck(maxRPM)){
		int_maxrpm = atoi(maxRPM);
	}else{
		int_maxrpm = 0;
		return FORM_ERROR;
	}
	if(ValidMaxRPM()){
		return FORM_OK;
	}

	return FORM_ERROR;
}
/* Name:
 * Description:
 * Inputs:
 * Outputs:
 */
int  FormData::GetMaxRPM(void){
	return int_maxrpm;
}

/* Name:
 * Description:
 * Inputs:
 * Outputs:
 */
BYTE FormData::SetMinRPM(char* minRPM) {
	if(numericCheck(minRPM)){
		int_minrpm = atoi(minRPM);
	}else{
		int_minrpm = 0;
		return FORM_ERROR;
	}

	if(ValidMinRPM()){
		return FORM_OK;
	}
	return FORM_ERROR;
}

/* Name:
 * Description:
 * Inputs:
 * Outputs:
 */
int  FormData::GetMinRPM(void) {
	return int_minrpm;
}

/* Name:
 * Description:
 * Inputs:
 * Outputs:
 */
BYTE FormData::SetSteps(char * steps) {
	return FORM_OK;
}

/* Name:
 * Description:
 * Inputs:
 * Outputs:
 */
int FormData::GetSteps (void) {
	return int_steps;
}

/* Name:
 * Description:
 * Inputs:
 * Outputs:
 */


BYTE FormData::SetRotations(char* rotations) {
	if(numericCheck(rotations)){
		int_rotations = atoi(rotations);
	}else{
		int_rotations = 0;
		return FORM_ERROR;
	}
	if (ValidRotations()){
		return FORM_OK;
	}
	return FORM_ERROR;
}

/* Name:
 * Description:
 * Inputs:
 * Outputs:
 */

int FormData::GetRotations(void){
	return int_rotations * rotation_direction;
}

/* Name:
 * Description:
 * Inputs:
 * Outputs:
 */
BYTE FormData::SetDirection(char * dir){
	return FORM_OK;
}

/* Name:
 * Description:
 * Inputs:
 * Outputs:
 */
BYTE FormData::GetDirection(void){
	return direction;
}

/* Name:
 * Description:
 * Inputs:
 * Outputs:
 */



/* Name:
 * Description:
 * Inputs:
 * Outputs:
 */

bool FormData::checkValid(int number, int min, int max){
	if(number == 0 || number<min || number>max){
			return false;
		}
		return true;
}

bool FormData::ValidMinRPM(){
	return checkValid(int_minrpm, MIN_INPUT, MAX_RPM) && checkValid(int_maxrpm, MIN_INPUT, MAX_RPM) && checkValid(int_minrpm, MIN_INPUT, int_maxrpm);
}

bool FormData::ValidMaxRPM(){
	return checkValid(int_maxrpm, MIN_INPUT, MAX_RPM);
}

bool FormData::ValidRotations(){
	return checkValid(int_rotations, MIN_INPUT, MAX_ROTATIONS );
}

bool FormData::validData(){
	return ValidMaxRPM() && ValidMinRPM() && ValidRotations();
}

bool FormData::numericCheck(char* stringInput){
	for(int i = 0; i < strlen(stringInput); i++){
		if((stringInput[i] < 48 && stringInput[i] != 32 && stringInput[i] != 9) || stringInput[i] > 57){
			return false;
		}
	}
	return true;
}

BYTE FormData::setStopped(bool stop){
	stopped = stop;
	return FORM_OK;
}

bool FormData::getStopped(){
	return stopped;
}

BYTE FormData::setDirection(int direction){
	rotation_direction = direction;
	return FORM_OK;
}

BYTE FormData::setPostAvail(bool post){
	post_avail = post;
	return FORM_OK;
}

bool FormData::getPostAvail(){
	return post_avail;
}

BYTE FormData::setEStopped(bool stop){
    estop = stop;
    return FORM_OK;
}

bool FormData::getEStopped(){
    return estop;
}


BYTE FormData::Init(BYTE motor_mode){
	direction = UNSET;
	mode = motor_mode;
	int_maxrpm = 0;
	int_minrpm = 0;
	int_steps = 0;
	int_rotations = 0;
	stopped = false;
	direction = 1;
	post_avail = false;
    estop = false;
	return false;
	return FORM_OK;

}



