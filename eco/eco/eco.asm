.include "m32def.inc" //include define for ATmega32 controller

//==========DATA SEGMENT=================================================
.dseg
.org 0x0060	
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
reset:
.include "core_init.inc"	//clear sram, registers and stack init
//-----------Program start-------------------------
load_strings_to_SRAM //save strings from eeprom to ram

LCD_init
LCD_print_str hello //print 16 symbols after "hello" label in sram

//-----------MAIN----------------------------------
main:			
nop
rjmp main		
//-------------------------------------------------
//========EEPROM SEGMENT=================================================
.eseg
.org 0
ee_hello_msg:	.db "Hello World!    "
ee_CH4_msg:		.db "testень СН4     "
ee_temp_msg:	.db "Температура     "
ee_time_msg:	.db "Время           "
ee_coords_msg:	.db "Координаты      "
ee_sound_msg:	.db "Звук (5-измен.) "