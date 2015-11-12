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
 .equ count_of_ticks = 1	//count of ticks for prevent false activation macro

 //=======MACRO==========================================
 //-------Preventing false activation----------------------------
 .macro Anti_false_activation
 
 ldi YH,high(tick_count)
 ldi YL,low(tick_count)
 ld temp,Y+						//load to temp count of ticks
 ld r16,Y						//load to r16 number of last pressed button

 cpi r16,@0							//compare block
 brne end_of_activation_macro		//example on C language:
 cpi temp,count_of_ticks			//	if(last_btn == @0)
 brsh end_of_activation_macro		//	{
									//		if(tick > count_of_ticks)
									//		{
									//			tick = 0;
									//			do_something...
									//		}
									//		else
									//		{
									//			return of On_(button)_Click
									//		}
									//	}
									//	else
									//	{
									//		tick = 0;
									//		do_something...
									//	}
									//	last_btn=@0

 rjmp @1							//return of On_(some_btn)_Click

 end_of_activation_macro:			//label to end of macro for continue

 ldi temp,0							//saving to RAM
 ldi r16,@0
 ldi YH,high(tick_count)
 ldi YL,low(tick_count)
 st Y+,temp
 st Y,r16

 .endm
 //-------Initialization ports for working with keyboard---------
 .macro keyboard_init
 in temp,ddrb
 andi temp, (~(0 | (1<<KBD_1) | (1<<KBD_2) | (1<<KBD_3)))
 out ddrb,temp			//set KBD_1 KBD_2 KBD_3 as input

 in temp,portb
 ori temp, (0 | (1<<KBD_1) | (1<<KBD_2) | (1<<KBD_3))
 out portb,temp			//enable pull-ups for KBD_1 KBD_2 KBD_3

 in temp,ddrd
 ori temp, (0 | (1<<KBD_A) | (1<<KBD_B) | (1<<KBD_C) | (1<<KBD_D))
 out ddrd,temp			//set KBD_A KBD_B KBD_C KBD_D as output
 .endm
 //-------Check buttons------------------------------------------
 .macro keyboard_check

	in temp,pind		//set pins without corruption of other pins
	ori temp, 0 | (1<<KBD_A) | (1<<KBD_B) | (1<<KBD_C) | (1<<KBD_D)
	andi temp, (~(0 | (1<<KBD_A)))//set KBD_A as 0, and KBD_B,C,D as 1
	out portd,temp

		sbis pinb,KBD_3
		rcall On_asterisk_Click

		sbis pinb,KBD_2
		rcall On_2_Click

		sbis pinb,KBD_1
		rcall On_3_Click

	in temp,pind		//set pins without corruption of other pins
	ori temp, 0 | (1<<KBD_A) | (1<<KBD_B) | (1<<KBD_C) | (1<<KBD_D)
	andi temp, (~(0 | (1<<KBD_B)))//set KBD_B as 0, and KBD_A,C,D as 1
	out portd,temp

		sbis pinb,KBD_3
		rcall On_1_Click

		sbis pinb,KBD_2
		rcall On_5_Click

		sbis pinb,KBD_1
		rcall On_6_Click

	in temp,pind		//set pins without corruption of other pins
	ori temp, 0 | (1<<KBD_A) | (1<<KBD_B) | (1<<KBD_C) | (1<<KBD_D)
	andi temp, (~(0 | (1<<KBD_C)))//set KBD_C as 0, and KBD_A,B,D as 1
	out portd,temp

		sbis pinb,KBD_3
		rcall On_4_Click

		sbis pinb,KBD_2
		rcall On_8_Click

		sbis pinb,KBD_1
		rcall On_9_Click

	in temp,pind		//set pins without corruption of other pins
	ori temp, 0 | (1<<KBD_A) | (1<<KBD_B) | (1<<KBD_C) | (1<<KBD_D)
	andi temp, (~(0 | (1<<KBD_D)))//set KBD_D as 0, and KBD_A,B,C as 1
	out portd,temp

		sbis pinb,KBD_3
		rcall On_7_Click

		sbis pinb,KBD_2
		rcall On_0_Click

		sbis pinb,KBD_1
		rcall On_grid_Click

 .endm

 //=======PROCEDURES=====================================
 //-------On 1 click-----------------------------
 On_1_click:
 push temp
 push YH
 push YL
 push r16

 Anti_false_activation '1', return_On_1_Click

 return_On_1_Click:
 pop r16
 pop YL
 pop YH
 pop temp

 sei
 sbis pinb,KBD_3		//wait until button is turned off
 rjmp PC-1
 ret
 //-------On 2 click-----------------------------
 On_2_click:
 push temp
 push YH
 push YL
 push r16

 Anti_false_activation '2', return_On_2_Click

 menu_prev

 return_On_2_Click:
 pop r16
 pop YL
 pop YH
 pop temp

 sei
 sbis pinb,KBD_2		//wait until button is turned off
 rjmp PC-1
 ret
 //-------On 3 click-----------------------------
 On_3_click:
 push temp
 push YH
 push YL
 push r16

 Anti_false_activation '3', return_On_3_Click
 
 return_On_3_Click:
 pop r16
 pop YL
 pop YH
 pop temp

 sei
 sbis pinb,KBD_1		//wait until button is turned off
 rjmp PC-1
 ret
 //-------On 4 click-----------------------------
 On_4_click:
 push temp
 push YH
 push YL
 push r16

 Anti_false_activation '4', return_On_4_Click

 return_On_4_Click:
 pop r16
 pop YL
 pop YH
 pop temp

 sei
 sbis pinb,KBD_1		//wait until button is turned off
 rjmp PC-1
 ret
 //-------On 5 click-----------------------------
 On_5_click:
 push temp
 push YH
 push YL
 push r16

 Anti_false_activation '5', return_On_5_Click

 ldi YH, high(menu_location)
 ldi YL, low(menu_location)
 ld temp, Y			//load current menu page to temp
 cpi temp,5			//if current page is sound
 brne return_On_5_Click	//then continue, else return
 ldi YH,high(sound_status)	//
 ldi YL,low(sound_status)	//read sound status from RAM
 ld temp,Y

 sbrc temp,sound_general_status	//if sound is generally off then set sound on
 rjmp sound_off					//else set sound off

 rcall sound_alarm_enable
 rjmp return_On_5_Click

 sound_off:
 rcall sound_alarm_disable

 return_On_5_Click:
 pop r16
 pop YL
 pop YH
 pop temp

 sei
 sbis pinb,KBD_2		//wait until button is turned off
 rjmp PC-1
 ret
 //-------On 6 click-----------------------------
 On_6_click:
 push temp
 push YH
 push YL
 push r16

 Anti_false_activation '6', return_On_6_Click

 return_On_6_Click:
 pop r16
 pop YL
 pop YH
 pop temp

 sei
 sbis pinb,KBD_1		//wait until button is turned off
 rjmp PC-1
 ret
 //-------On 7 click-----------------------------
 On_7_click:
 push temp
 push YH
 push YL
 push r16

 Anti_false_activation '7', return_On_7_Click

 return_On_7_Click:
 pop r16
 pop YL
 pop YH
 pop temp

 sei
 sbis pinb,KBD_3		//wait until button is turned off
 rjmp PC-1
 ret
 //-------On 8 click-----------------------------
 On_8_click:
 push temp
 push YH
 push YL
 push r16

 Anti_false_activation '8', return_On_8_Click
 
 menu_next

 return_On_8_Click:
 pop r16
 pop YL
 pop YH
 pop temp

 sei
 sbis pinb,KBD_2		//wait until button is turned off
 rjmp PC-1
 ret
 //-------On 9 click-----------------------------
 On_9_click:
 push temp
 push YH
 push YL
 push r16

 Anti_false_activation '9', return_On_9_Click

 return_On_9_Click:
 pop r16
 pop YL
 pop YH
 pop temp

 sei
 sbis pinb,KBD_1		//wait until button is turned off
 rjmp PC-1
 ret
 //-------On * click-----------------------------
 On_asterisk_click:
 push temp
 push YH
 push YL
 push r16

 Anti_false_activation '*', return_On_asterisk_Click

 return_On_asterisk_Click:
 pop r16
 pop YL
 pop YH
 pop temp

 sei
 sbis pinb,KBD_3		//wait until button is turned off
 rjmp PC-1
 ret
 //-------On 0 click-----------------------------
 On_0_click:
 push temp
 push YH
 push YL
 push r16

 Anti_false_activation '0', return_On_0_Click

 return_On_0_Click:
 pop r16
 pop YL
 pop YH
 pop temp

 sei
 sbis pinb,KBD_2		//wait until button is turned off
 rjmp PC-1
 ret
 //-------On # click-----------------------------
 On_grid_click:
 push temp
 push YH
 push YL
 push r16

 Anti_false_activation '#', return_On_grid_Click

 return_On_grid_Click:
 pop r16
 pop YL
 pop YH
 pop temp

 sei
 sbis pinb,KBD_3		//wait until button is turned off
 rjmp PC-1
 ret
 //----------------------------------------------
//================================================================
//                 Sorry for bad english