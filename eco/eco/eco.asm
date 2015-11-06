.include "m32def.inc" //include define for ATmega32 controller

 .def temp=r17		   //define r16 as temp
 .def byte=r16		   //define r17 as byte (will be used for send byte to LCD)
 .def menu_loc=r18

//==========DATA SEGMENT=================================================
// In this segment we can to reserve some memory
// for strings for example...
.dseg
.org 0x0060						//starting from 0x0060 because 
								//before that adress we have reserved memory for registers
CO_msg:				.byte 16	//hello world string
CH4_msg:			.byte 16	//CH4 level message string
temperature_msg:	.byte 16	//Temperature message string
time_msg:			.byte 16	//Time message string
coords_msg:			.byte 16	//Coordinates message string
sound_msg:			.byte 16	//Sound option message string
test:				.byte 96	//reserve bytes for 2nd string messages
menu_location:		.byte 1		//current menu location
tick_count:			.byte 1		//count of button activations
last_btn:			.byte 1		//number of last pressed button

//==========CODE SEGMENT=================================================
.cseg
.include "int_table.inc"	//interrupt vectors
.include "menu.inc"			//macro for menu uses
.include "LCD.inc"			//macro and procedures for HD44780 like displays
.include "keyboard.inc"		//macro and procedures for 3x4 keyboard
.include "timers.inc"

reset:
.include "core_init.inc"	//clear sram, registers and stack init
//-----------Program start-------------------------

Timer1_Init
keyboard_init				//initialization of ports for keyboard
LCD_init					//LCD initialization
menu_init

//-----------MAIN----------------------------------
main:			
	nop
rjmp main		
//-------------------------------------------------
//========EEPROM SEGMENT=================================================
.eseg
.org 0
ee_hello_msg:	.db "Level CO        "
ee_CH4_msg:		.db "Level CH4      ",0
ee_temp_msg:	.db "Temperature    ",0
ee_coords_msg:	.db "Coordinates    ",0
ee_time_msg:	.db "Time           ",0
ee_sound_msg:	.db "Sound(5-On/Off)",0
ee_test_msg1:	.db "test str 1     ",1
ee_test_msg2:	.db "test str 2     ",1
ee_test_msg3:	.db "test str 3     ",1
ee_test_msg4:	.db "test str 4     ",1
ee_test_msg5:	.db "test str 5     ",1
ee_test_msg6:	.db "test str 6      "
//=======================================================================
//                   Sorry for bad english