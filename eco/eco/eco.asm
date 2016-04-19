.include "m32def.inc" //include define for ATmega32 controller

 .equ freq=16000000	   //frequency of microcontoller
 .def temp=r17		   //define r16 as temp
 .def byte=r16		   //define r17 as byte (will be used for send byte to LCD)
 .def menu_loc=r18
 .equ XTAL = 16000000 //or 16000000	//microcontroller frequency

//==========DATA SEGMENT=================================================
.include "DSEG.inc"
//==========CODE SEGMENT=================================================
.cseg
.include "int_table.inc"	//interrupt vectors
.include "delay.inc"		//delay macro and proc
.include "LCD.inc"			//macro and procedures for HD44780 like displays
.include "menu.inc"			//macro for menu uses
.include "I2C_Real_Time_Clock.inc"	//working with LCD
.include "TimeSet.inc"		//time set procedures and macro
.include "sound.inc"		//macro and procedures for buzzer sound
.include "keyboard.inc"		//macro and procedures for 3x4 keyboard
.include "timers.inc"		//macro and procedures for timer(s)
//.include "softUART.inc"		//software UART
.include "UART.inc"
.include "strings.inc"
.include "GSM.inc"

reset:
.include "core_init.inc"	//clear sram, registers and stack init
//-----------Program start-------------------------

Timer1_Init					//initialization timer 1
keyboard_init				//initialization of ports for keyboard
LCD_init					//LCD initialization
menu_init					//menu initialization
sound_init					//buzzer initialization
I2C_Init					//i2c initialization
I2C_Check_Clock_Settings	//check clock settings and correct if needed
time_set_init
UART_init
call GSM_init_strings
UART_start_receive

//-----------MAIN----------------------------------
main:

	call GSM_main
	Refresh_time
	menu_refresh

rjmp main		
//-------------------------------------------------
//========EEPROM SEGMENT=================================================
.eseg
.include "EEPROM.inc"
//=======================================================================
//                   Sorry for bad english