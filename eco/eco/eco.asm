.include "m32def.inc" //include define for ATmega32 controller


 .macro dbg
sbi PORTD, 7
delay 82, 43, 77		//delay 1s
cbi PORTD, 7
delay 82, 43, 77		//delay 1s
.endm

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
//.include "math.inc"
.include "delay.inc"		//delay macro and proc
.include "LCD.inc"			//macro and procedures for HD44780 like displays
.include "menu.inc"			//macro for menu uses
.include "I2C.inc"
.include "RTC.inc"
.include "TimeSet.inc"		//time set procedures and macro
.include "sound.inc"		//macro and procedures for buzzer sound
.include "keyboard.inc"		//macro and procedures for 3x4 keyboard
.include "timers.inc"		//macro and procedures for timer(s)
//.include "softUART.inc"		//software UART
.include "UART.inc"
.include "strings.inc"
.include "GSM.inc"
.include "Temp.inc"
//.include "bmp.inc"

reset:
.include "core_init.inc"	//clear sram, registers and stack init
//-----------Program start-------------------------
sbi DDRD, 7
Timer1_Init					//initialization timer 1
keyboard_init				//initialization of ports for keyboard
call LCD_init				//LCD initialization
menu_init					//menu initialization
sound_init					//buzzer initialization
I2C_Init					//i2c initialization
I2C_Check_Clock_Settings	//check clock settings and correct if needed
time_set_init
UART_init
GSM_init

/*ldi YH, high(testtest)
ldi YL, low(testtest)
ldi temp, 0xFF
st Y, temp

.MACRO stw
ldi temp, high(@1)
sts @0, temp
ldi temp, low(@1)
sts @0+1, temp
.endm

stw AC1, 0x0198
stw AC2, 0xffb8
stw AC3, 0xc7d1
stw AC4, 0x7FE5
stw AC5, 0x7FF5
stw AC6, 0x5A71
stw B1, 0x182E
stw B2, 0x0004
stw MB, 0x8000
stw MC, 0xDDF9
stw MD, 0x0B34
stw UT, 0x6CFA
stw UP, 0x5D23*/

/*ldi temp, 0x12
sts X1, temp
ldi temp, 0x34
sts X1+1, temp

ldi temp, 0x56
sts X1+2, temp
ldi temp, 0x78
sts X1+3, temp

ldi temp, 0x87
sts X2, temp
ldi temp, 0x65
sts X2+1, temp

ldi temp, 0x43
sts X2+2, temp
ldi temp, 0x21
sts X2+3, temp

tmul32 X1, X2, T
muli32 X2, 0x12345678, X3
mul16 X1, X2, B5
muli16 X1, 0x4321, B6

//swldw X1, 10, X2
//mulwdw X1+2, X1, X1

call bmp_calculate*/

//-----------MAIN----------------------------------
main:

	call GSM_main
	Refresh_time
	call Temperature
	menu_refresh
	delay 255,255,255		//long delay

rjmp main		
//-------------------------------------------------
//========EEPROM SEGMENT=================================================
.eseg
.include "EEPROM.inc"
//=======================================================================
//                   Sorry for bad english