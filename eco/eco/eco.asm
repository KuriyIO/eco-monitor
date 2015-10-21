.include "m32def.inc" //include define for ATmega32 controller

 .def temp=r17		   //define r16 as temp
 .def byte=r16		   //define r17 as byte (will be used for send byte to LCD)

//==========DATA SEGMENT=================================================
// In this segment we can to reserve some memory
// for strings for example...
.dseg
.org 0x0060						//starting from 0x0060 because 
								//before that adress we have reserved memory for registers
hello:				.byte 16	//hello world string
CH4_msg:			.byte 16	//CH4 level message string
temperature_msg:	.byte 16	//Temperature message string
time_msg:			.byte 16	//Time message string
coords_msg:			.byte 16	//Coordinates message string
sound_msg:			.byte 16	//Sound option message string

//==========CODE SEGMENT=================================================
.cseg
.include "int_table.inc"	//interrupt vectors
.include "LCD.inc"			//macro and procedures for HD44780 like displays
.include "keyboard.inc"		//macro and procedures for 3x4 keyboard
reset:
.include "core_init.inc"	//clear sram, registers and stack init
//-----------Program start-------------------------
load_strings_to_SRAM		//save strings from eeprom to ram

keyboard_init				//initialization of ports for keyboard
LCD_init					//LCD initialization

//-----------MAIN----------------------------------
main:			
	key_survay		//check for buttons press
rjmp main		
//-------------------------------------------------
//========EEPROM SEGMENT=================================================
.eseg
.org 0
ee_hello_msg:	.db "Hello World!    "
ee_CH4_msg:		.db "Уровень СН4     "
ee_temp_msg:	.db "Температура     "
ee_time_msg:	.db "Время           "
ee_coords_msg:	.db "Координаты      "
ee_sound_msg:	.db "Звук (5-измен.) "
//=======================================================================
//                   Sorry for bad english