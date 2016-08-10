/*
 * FormData.h
 *
 *  Created on: Oct 29, 2009
 *      Author: Nancy Minderman
 *      Edited by Rajan Jassal 2014
 */

#ifndef FORMDATA_H_
#define FORMDATA_H_

#include <basictypes.h>
#include <ucos.h>

#define FORM_NO_DATA 	0xFF
#define FORM_ERROR 		0xFE
#define FORM_OK			0

enum dir { 	UNSET = 0,
		CW = 1,
		CCW = 2,
		STOP = 3
};


class FormData {
public:
	FormData();
	virtual ~FormData();
	BYTE SetMaxRPM(char* maxRPM);
	int  GetMaxRPM(void);
	BYTE SetMinRPM(char* minRPM);
	int  GetMinRPM(void);
	BYTE SetSteps(char * st);
	int  GetSteps (void);
	BYTE SetRotations(char* rotations);
	int  GetRotations(void);
	BYTE SetDirection(char * dir);
	BYTE GetDirection(void);
	int GetMode(void);
	BYTE Init(BYTE motor_mode);
	bool ValidMinRPM(void);
	bool ValidMaxRPM(void);
	bool ValidRotations(void);
	BYTE setStopped(bool stop);
	bool getStopped();
	BYTE setDirection(int direction);
	bool validData();
	BYTE setPostAvail(bool post);
	bool getPostAvail();
    BYTE setEStopped(bool stop);
    bool getEStopped();

private:
	bool checkValid(int number, int min, int max);
	bool numericCheck(char* stringInput);
	int  int_maxrpm;
	int  int_minrpm;
	int  int_steps;
	int	 int_rotations;
	BYTE direction;
	int mode;
	bool stopped;
	int rotation_direction;
	bool post_avail;
    bool estop;
};

#endif /* FORMDATA_H_ */
