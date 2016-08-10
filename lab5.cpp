/* Rev:$Revision: 1.1 $ */
/******************************************************************************
 * Copyright 1998-2008 NetBurner, Inc.  ALL RIGHTS RESERVED
 *   Permission is hereby granted to purchasers of NetBurner Hardware
 *   to use or modify this computer program for any use as long as the
 *   resultant program is only executed on NetBurner provided hardware.
 *
 *   No other rights to use this program or it's derivitives in part or
 *   in whole are granted.
 *
 *   It may be possible to license this or other NetBurner software for
 *   use on non NetBurner Hardware. Please contact Licensing@Netburner.com
 *   for more information.
 *
 *   NetBurner makes no representation or warranties with respect to the
 *   performance of this computer program, and specifically disclaims any
 *   responsibility for any damages, special or consequential, connected
 *   with the use of this program.
 *
 *   NetBurner, Inc
 *   5405 Morehouse Drive
 *   San Diego Ca, 92121
 *   http://www.netburner.com
 *
 *****************************************************************************/

#include "predef.h"
#include <stdio.h>
#include <ctype.h>
#include <startnet.h>
#include <autoupdate.h>
#include <taskmon.h>
#include <dhcpclient.h>
#include <string.h>
#include "stepper.h"
#include "keypad.h"
#include <eTPU.h>
#include <ETPUInit.h>
#include <eTPU_sm.h>
#include <sim5234.h>
#include <cfinter.h>
#include "lcd.h"
#include "motorconstants.h"
#include "formdata.h"


const char *AppName = "Aaron and Jassal Lab 5\n";
extern "C"
{
   void UserMain( void *pd );
   void DisplayLameCounter( int sock, PCSTR url );
   void MinRPM(int sock, PCSTR url);
   void MaxRPM(int sock, PCSTR url);
   void Rotations(int sock, PCSTR url);
   void MotorMode(int sock, PCSTR url);
   void IRQIntInit(void);
   void SetIntc(int intc, long func, int vector, int level, int prio);
}
extern void RegisterPost();

FormData myData;
OS_SEM form_sem;
Keypad myKeypad;
Lcd myLCD;
Stepper myStepper(SM_MASTER_CHANNEL, SM_ACCEL_TABLE_SIZE);
#define MAX_COUNTER_BUFFER_LENGTH 100
int status = 0;
bool estop = false;

void UserMain( void *pd )
{
	BYTE err = OS_NO_ERR;
	InitializeStack();
	OSChangePrio( MAIN_PRIO );
	EnableAutoUpdate();

	eTPUInit();
	OSSemInit(&form_sem, 1);
	myLCD.Init(LCD_BOTH_SCR);
	myKeypad.Init();


	/* Initialise your formdata and stepper class here based on the output
	 * from the DIP switches.
	 */
	int mode;
	int stepConversion;

	if(getdipsw()>0){
		mode = ECE315_ETPU_SM_FULL_STEP_MODE;
		stepConversion = 100;
	}else {
		mode = ECE315_ETPU_SM_HALF_STEP_MODE;
		stepConversion = 200;
	}
	myStepper.Init(mode,
				   SM_MAX_PERIOD,
				   SM_INIT_SLEW_PERIOD);

	StartHTTP();
	EnableTaskMonitor();
	IRQIntInit();

	//Call a registration function for our Form code
	// so POST requests are handled properly.
	RegisterPost();

	myLCD.Clear(LCD_BOTH_SCR);
	myLCD.PrintString(LCD_UPPER_SCR, "Welcome to Lab 5 - ECE315");
	OSTimeDly(TICKS_PER_SECOND*1);

	while ( 1 )
	{

		printf("%d\n", status);
		OSSemPend(&form_sem, 0);

		if(myData.getStopped()){
			myLCD.MoveCursor(LCD_UPPER_SCR,40);
			myLCD.PrintString(LCD_UPPER_SCR, "Stop requested");
			myStepper.Stop();
			myData.setStopped(false);
		}else if(myData.getEStopped()){
			myLCD.MoveCursor(LCD_LOWER_SCR,0);
			myLCD.PrintString(LCD_LOWER_SCR, "Emergency Stop requested");
			myStepper.Stop();
            myData.setEStopped(false);
		}else if (myData.validData() && myData.getPostAvail()){
			myLCD.Clear(LCD_BOTH_SCR);
			if(myData.GetRotations()){
				myLCD.PrintString(LCD_UPPER_SCR,"Direction == CW");
			}else{
				myLCD.PrintString(LCD_UPPER_SCR,"Direction == CWW");
			}
			myStepper.SetSlewPeriodUsingRPM(myData.GetMaxRPM());
			myStepper.SetStartPeriodUsingRPM(myData.GetMinRPM());
			myStepper.Step(myData.GetRotations() * stepConversion);
			myData.setPostAvail(false);

		}
		OSSemPost(&form_sem);
		//myStepper.Step(100);// cw movement 100 steps = 1 rotation in full step mode

		//OSTimeDly(TICKS_PER_SECOND*3);
		//myStepper.Step(-100); // ccw movement 100 steps = 1 rotation in full step mode
		OSTimeDly(TICKS_PER_SECOND*3);
	}
}


/* Name: DisplayLameCounter
 * Description: Displays the number of times the page has been (re)loaded.
 * Meant to be called using <!-- FUNCTIONCALL DisplayLameCounter --> in the
 * index.htm file that contains our forms.
 * Inputs: int sock is a file descriptor for the socket that we are
 * using to communicate with the client.
 * Outputs: None
 */
void DisplayLameCounter( int sock, PCSTR url )
{
	static int form_counter = 0;
	char buffer[MAX_COUNTER_BUFFER_LENGTH+1];

	if((sock > 0) && (url != NULL)) {
		iprintf(url);
		snprintf(buffer,MAX_COUNTER_BUFFER_LENGTH, "<H1>The page has been reloaded %d times. </H1>", form_counter );
		form_counter++;
		writestring(sock,(const char *) buffer);

	}
}

void MinRPM(int sock, PCSTR url){
	//static char input[MAX_INPUT_LENGTH+1] = "";
	//static int minRPM = 0;
	char buffer[MAX_COUNTER_BUFFER_LENGTH+1];
	char validIMG[] = "http://oi61.tinypic.com/2u9su1f.jpg";
	char notValidIMG[] = "img4.wikia.nocookie.net/__cb20131211220605/ssb/images/5/54/Failure.png";
	char* displayed_image;

	OSSemPend(&form_sem, WAIT_FOREVER);
	if(myData.ValidMinRPM()){
		displayed_image = validIMG;
	}else{
		displayed_image = notValidIMG;
	}
	OSSemPost(&form_sem);

	if((sock >0) && (url != NULL)){
		snprintf(buffer,MAX_COUNTER_BUFFER_LENGTH, "<IMG src =\"%s\" alt=\"Good\" align=\"right\"/>", displayed_image);
		writestring(sock,(const char *) buffer);
	}
}

void MaxRPM(int sock, PCSTR url){
	//static char input[MAX_INPUT_LENGTH+1] = "";
	//static int minRPM = 0;
	char buffer[MAX_COUNTER_BUFFER_LENGTH+1];
	char validIMG[] = "http://worterbuchmitbedeutung.com/img/valid.png";
	char notValidIMG[] = "http://images.clipartpanda.com/wrong-clipart-7iaLbGKiA.png";
	char* displayed_image;

	OSSemPend(&form_sem, WAIT_FOREVER);
	if(myData.ValidMaxRPM()){
		displayed_image = validIMG;
	}else{
		displayed_image = notValidIMG;
	}
	OSSemPost(&form_sem);

	if((sock >0) && (url != NULL)){
		snprintf(buffer,MAX_COUNTER_BUFFER_LENGTH, "<IMG src =\"%s\" alt=\"Good\" align=\"right\"/>",displayed_image);
		writestring(sock,(const char *) buffer);
	}
}

void Rotations(int sock, PCSTR url){
	char buffer[MAX_COUNTER_BUFFER_LENGTH+1];
	char validIMG[] = "http://worterbuchmitbedeutung.com/img/valid.png";
	char notValidIMG[] = "http://images.clipartpanda.com/wrong-clipart-7iaLbGKiA.png";
	char* displayed_image;

	OSSemPend(&form_sem, WAIT_FOREVER);
	if(myData.ValidRotations()){
		displayed_image = validIMG;
	}else{
		displayed_image = notValidIMG;
	}
	OSSemPost(&form_sem);

	if((sock >0) && (url != NULL)){
		snprintf(buffer,MAX_COUNTER_BUFFER_LENGTH, "<IMG src =\"%s\" alt=\"Good\" align=\"right\"/>", displayed_image );
		writestring(sock,(const char *) buffer);
	}
}


void MotorMode(int sock, PCSTR url){
	char buffer[MAX_COUNTER_BUFFER_LENGTH+1];

	if(getdipsw()){
		snprintf(buffer,MAX_COUNTER_BUFFER_LENGTH, "ECE 315 Full Step Motor Controller");
	}
	else{
		snprintf(buffer,MAX_COUNTER_BUFFER_LENGTH, "ECE 315 Half Step Motor Controller");
	}
	writestring(sock,(const char *) buffer);
}

INTERRUPT(out_irq_pin_isr, 0x2500)
{
	if(myKeypad.ButtonPressed()){
       myData.setEStopped(true);
	}

	sim.eport.epfr |= 8;
}

void IRQIntInit(void) {
	// 0x4040000013 0x0000000015 0x880016
	sim.eport.eppar |= 0x0040; //look at chapter 15 tables not zero
	sim.eport.epddr &= !0x8;
	sim.eport.epier |= 0x08;
	SetIntc(0, (long int) out_irq_pin_isr, 3, 1, 1);

}






