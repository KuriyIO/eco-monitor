/*
 * keyboard.inc
 * macro and procedures for 3x4 keyboard
 * Sorry for bad english
 */ 

 .equ KBD_A = 2
 .equ KBD_B = 3
 .equ KBD_C = 4
 .equ KBD_D = 5
 .equ KBD_1 = 0
 .equ KBD_2 = 1
 .equ KBD_3 = 2

 //=======PROCEDURES=====================================
 //-------On 1 click-----------------------------
 On_1_click:
 sbis pind,2		//wait until button is turned off
 rjmp PC-1
 ret
 //-------On 2 click-----------------------------
 On_2_click:
 menu_prev 
 sbis pind,2		//wait until button is turned off
 rjmp PC-1
 ret
 //-------On 3 click-----------------------------
 On_3_click:
 sbis pind,2		//wait until button is turned off
 rjmp PC-1
 ret
 //-------On 4 click-----------------------------
 On_4_click:
 sbis pind,3		//wait until button is turned off
 rjmp PC-1
 ret
 //-------On 5 click-----------------------------
 On_5_click:
 sbis pind,3		//wait until button is turned off
 rjmp PC-1
 ret
 //-------On 6 click-----------------------------
 On_6_click:
 sbis pind,3		//wait until button is turned off
 rjmp PC-1
 ret
 //-------On 7 click-----------------------------
 On_7_click:
 sbis pind,4		//wait until button is turned off
 rjmp PC-1
 ret
 //-------On 8 click-----------------------------
 On_8_click:
 menu_next
 sbis pind,4		//wait until button is turned off
 rjmp PC-1
 ret
 //-------On 9 click-----------------------------
 On_9_click:
 sbis pind,4		//wait until button is turned off
 rjmp PC-1
 ret
 //-------On 0 click-----------------------------
 On_0_click:
 LCD_clear
 sbis pind,5		//wait until button is turned off
 rjmp PC-1
 ret
 //-------On * click-----------------------------
 On_asterisk_click:
 sbis pind,5		//wait until button is turned off
 rjmp PC-1
 ret
 //-------On # click-----------------------------
 On_grid_click:
 sbis pind,5		//wait until button is turned off
 rjmp PC-1
 ret
 //----------------------------------------------
 //=======MACRO==========================================
 //-------Initialization ports for working with keyboard---------
 .macro keyboard_init
 ldi temp,0b0000_0111	//set KBD_1 KBD_2 KBD_3 as output
 out ddrb,temp
 ldi temp,0b1000_0000	//set KBD_A KBD_B KBD_C KBD_D as input
 out ddrd,temp
 ldi temp,0b0011_1100	//activate pull-ups
 out portd,temp
 .endm
 //-------Check buttons------------------------------------------
 .macro keyboard_check

	ldi temp,0b0000_0110		//check 1st column
	out portb,temp

		sbis pind,KBD_A			//sbic - skip if bit in port is set
		rcall On_1_click		//if bit 2 in pind is 1 this instruction 
								//will be skipped
		sbis pind,KBD_B			//that mean if button was pressed
		rcall On_4_click		//the next instruction is call procedure
								//which do something what you need...
		sbis pind,KBD_C			//and after that program counter going back
		rcall On_7_click		//to key survay

		sbis pind,KBD_D
		rcall On_asterisk_click

	ldi temp,0b0000_0101		//check 2nd column
	out portb,temp

		sbis pind,KBD_A			//the same like in previous
		rcall On_2_click		//but for next column

		sbis pind,KBD_B
		rcall On_5_click

		sbis pind,KBD_C
		rcall On_8_click

		sbis pind,KBD_D
		rcall On_0_click


	ldi temp,0b0000_0011		//check 3rd column
	out portb,temp

		sbis pind,KBD_A
		rcall On_3_click

		sbis pind,KBD_B
		rcall On_6_click

		sbis pind,KBD_C
		rcall On_9_click

		sbis pind,KBD_D
		rcall On_grid_click

 .endm
//================================================================
//                 Sorry for bad english