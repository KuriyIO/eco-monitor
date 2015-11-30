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
CO_msg:				.byte 16	//level CO string
CH4_msg:			.byte 16	//CH4 level message string
temperature_msg:	.byte 16	//Temperature message string
coords_msg:			.byte 16	//Coordinates message string
time_msg:			.byte 16	//Time message string
sound_msg:			.byte 16	//Sound option message string
CO_value:			.byte 16	//string which contain level CO value
CH4_value:			.byte 16	//string which contain level CH4 value
temperature_value:	.byte 16	//string which contain temperature value
coords_value:		.byte 16	//string which contain coordinates value
time_value:			.byte 16	//string which contain time value
sound_value:		.byte 16	//string which contain sound On/Off value
menu_location:		.byte 1		//current menu location
tick_count:			.byte 1		//count of button activations
last_btn:			.byte 1		//number of last pressed button
sound_status:		.byte 1		//status of sound
I2C_status:			.byte 1		//I2C status register
I2C_buffer:			.byte 5		//I2C buffer
I2C_device:			.byte 1		//I2C device adress + R/W
I2C_data_pointer:	.byte 1		//pointer to clock's register
I2C_data_read:		.byte 1		//readed data
I2C_data_write:		.byte 1		//data which need to write
time_set_status:	.byte 1		//time set status register
time_set_value:		.byte 1		//contain a value which need to set
time_set_ranges:	.byte 10	//contain a table with max values range
time_set_max_values:.byte 5		//caontain a max values of hh:mm dd/MM/yy
time_set_buffer:	.byte 2		//contain a active pair values
time_set_blink_count:.byte 1	//count of tick before blink

//==========CODE SEGMENT=================================================
.cseg
.include "int_table.inc"	//interrupt vectors
.include "LCD.inc"			//macro and procedures for HD44780 like displays
.include "menu.inc"			//macro for menu uses
.include "I2C_Real_Time_Clock.inc"	//working with LCD
.include "TimeSet.inc"		//time set procedures and macro
.include "sound.inc"		//macro and procedures for buzzer sound
.include "keyboard.inc"		//macro and procedures for 3x4 keyboard
.include "timers.inc"		//macro and procedures for timer(s)

reset:
.include "core_init.inc"	//clear sram, registers and stack init
//-----------Program start-------------------------

Timer1_Init
keyboard_init				//initialization of ports for keyboard
LCD_init					//LCD initialization
menu_init
sound_init
I2C_Init
I2C_Check_Clock_Settings
time_set_init

//-----------MAIN----------------------------------
main:

	Refresh_time
	menu_refresh

	
    ldi  r18, 5
    ldi  r19, 194
    ldi  r20, 134
    ldi  r21, 33
L1: dec  r21
    brne L1
    dec  r20
    brne L1
    dec  r19
    brne L1
    dec  r18
    brne L1
	
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
ee_test_msg5:	.db "12:34 14/11/15 ",1
ee_test_msg6:	.db "Press 5 to test "
//=======================================================================
//                   Sorry for bad english