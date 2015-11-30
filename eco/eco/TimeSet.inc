/*
 * TimeSet.inc
 *
 *  Created: 17.11.2015 1:07:04
 *   Author: Hogs
 */ 
 .equ time_set_S = 7		//time set status flag
 .equ time_set_F = 6		//time set first number flag
 .equ time_set_B = 5		//time set blink flag

 //======PROCEDURES==========================================
 //-----------------------------------------------------
 time_set_blink:
 push temp
 push YH
 push YL
 push r16

 ldi YH,high(time_set_status)		//read time_set status
 ldi YL,low(time_set_status)
 ld r16,Y

 sbrs r16,time_set_S				//if time set mode is off
 rjmp return_time_set_blink			//then return

									//continue if time set mode is On

 ldi YH,high(time_set_blink_count)	//time_set_blink_count is a counter
 ldi YL,low(time_set_blink_count)	//how much timer was ticked before
 ld temp,Y							//pair visible On/Off

 cpi temp,10						//if timer ticked 10 times or more - continue
 brlo return_time_set_blink			//else - return

 ldi YH,high(time_set_blink_count)
 ldi YL,low(time_set_blink_count)
 clr temp							//clear time_set_blink_counter
 st Y,temp

 sbrs r16,time_set_B			//check blink flag
 rjmp blink_pair				//if blink flag is 0 then pair is visible,
								//if blink flag is 1 then pair is "  "

 ldi YH,high(time_set_status)
 ldi YL,low(time_set_status)
 andi r16, ~(1<<time_set_B)			//set blink flag as 0
 st Y,r16

 rcall time_set_get_pair_adress		//get to Y register adress of symbols for current pair
 ldi temp, ' '				//set current pair as "  " so it is like invisible
 st Y+,temp
 st Y,temp

 rjmp return_time_set_blink	//return

 blink_pair:

 ldi YH,high(time_set_status)
 ldi YL,low(time_set_status)
 ori r16, (1<<time_set_B)		//set blink flag as 1
 st Y,r16

 push ZH
 push ZL
 rcall time_set_get_pair_adress	//get to Y register symbol's adress of current pair
 ldi ZH,high(time_set_buffer)	
 ldi ZL,low(time_set_buffer)
 ld temp,Z+						//returning from time_set_buffer symbols which was before clear
 st Y+,temp
 ld temp,Z
 st Y,temp
 pop ZL
 pop ZH


 return_time_set_blink:
 menu_refresh		//refresh menu

 pop r16
 pop YL
 pop YH
 pop temp
 ret
 //-----------------------------------------------------
 time_set_set_buffer:
 push YH
 push YL
 push ZH
 push ZL
 push temp

 ldi ZH,high(time_set_buffer)
 ldi ZL,low(time_set_buffer)

 rcall time_set_get_pair_adress
 ld temp,Y+
 st Z+,temp
 ld temp,Y
 st Z,temp

 pop temp
 pop ZL
 pop ZH
 pop YL
 pop YH
 ret

 //-----------------------------------------------------
 time_set_get_pair_adress:
 push temp
 push r16

 ldi YH,high(time_set_status)
 ldi YL,low(time_set_status)
 ld temp,Y

 andi temp,0b0001_1111		//clear flags
 mov r16,temp	
 add temp,r16				//temp = pair*2
 add temp,r16				//temp = pair*2 + pair

 ldi YH,high(time_value)
 ldi YL,low(time_value)

 add YL,temp
 clr r16
 adc YH,r16

 pop r16
 pop temp
 ret
 //---------------------------------------------
 time_set_next_pair:
 push YH
 push YL
 push temp

 push ZH
 push ZL
 rcall time_set_get_pair_adress	//get to Y register symbol's adress of current pair
 ldi ZH,high(time_set_buffer)	
 ldi ZL,low(time_set_buffer)
 ld temp,Z+						//returning from time_set_buffer symbols which was before clear
 st Y+,temp
 ld temp,Z
 st Y,temp
 pop ZL
 pop ZH

 ldi YH,high(time_set_status)
 ldi YL,low(time_set_status)
 ld temp,Y

 andi temp,~(1<<time_set_F)

 push temp
 
 andi temp,0b0001_1111
 cpi temp,4
 brsh time_set_next_pair_clear

 pop temp
 inc temp
 rjmp time_set_next_pair_return

 time_set_next_pair_clear:
 pop temp
 andi temp,0b1110_0000

 time_set_next_pair_return:
 st Y,temp
 rcall time_set_set_buffer

 pop temp
 pop YL
 pop YH
 ret
 //-------Time set mode toggle--------------------
 time_set_start:
 
 ldi YH,high(time_set_status)
 ldi YL,low(time_set_status)
 ld temp,Y

 sbrs temp,time_set_S
 rjmp enable_time_set


 push ZH
 push ZL
 rcall time_set_get_pair_adress	//get to Y register symbol's adress of current pair
 ldi ZH,high(time_set_buffer)	
 ldi ZL,low(time_set_buffer)
 ld temp,Z+						//returning from time_set_buffer symbols which was before clear
 st Y+,temp
 ld temp,Z
 st Y,temp
 pop ZL
 pop ZH

 ldi YH,high(time_set_status)
 ldi YL,low(time_set_status)
 ldi temp, 0x00
 st Y,temp
 rcall String_to_time
 rcall I2C_Clock_Set_New_Time
 Refresh_time

 rjmp return_time_set_start

 enable_time_set:

 rcall time_set_set_buffer	//save values of current pair to buffer
 ldi temp,(1<<time_set_S)
 st Y,temp

 return_time_set_start:
 menu_refresh

 ret

 //-----------------------------------------
 time_set_set_next_pair:
 
 ldi YH,high(time_set_status)
 ldi YL,low(time_set_status)
 ld temp,Y

 sbrs temp,time_set_S
 rjmp return_time_set_set_next_pair

 rcall time_set_next_pair

 return_time_set_set_next_pair:
 menu_refresh

 ret
 //-----------------------------------------
 time_set:
 push temp
 push YH
 push YL
 push r16

 push ZH
 push ZL
 rcall time_set_get_pair_adress	//get to Y register symbol's adress of current pair
 ldi ZH,high(time_set_buffer)	
 ldi ZL,low(time_set_buffer)
 ld temp,Z+						//returning from time_set_buffer symbols which was before clear
 st Y+,temp
 ld temp,Z
 st Y,temp
 pop ZL
 pop ZH

 ldi YH,high(time_set_status)
 ldi YL,low(time_set_status)
 ld temp,Y						//read time_set_status to temp

 sbrs temp,time_set_S	//return if time set status is off(0)
 rjmp return_time_set	//if time set status is 1 then continue

 //time set mode is on
 sbrc temp,time_set_F			//skip if number is second
 rjmp time_set_second_number	//continue if number is first

 //number is first

 ld r16,Y
 andi r16, ~((1<<time_set_S)|(1<<time_set_F)|(1<<time_set_B))	//now r16 is a number of pair
 ldi YH,high(time_set_ranges)
 ldi YL,low(time_set_ranges)	//load to Y adress of max ranges
 add r16,r16
 add YL,r16						//now Y is max ranges for current pair
 clr r16		//clear r16, for next operation
 adc YH,r16
 ld r16,Y		//now r16 = max range for first number of this pair

 mov temp,r18		//temp = ASCII code of number
 subi temp,48		//now temp = number
 cp temp,r16	//if number > max range, then continue
 breq time_set_first_lower_first	//if number = max range, then go to label
 brlo time_set_first_lower_first	//if number < max range, then go to label

 //if number is higher than max range
 rcall time_set_get_pair_adress
 ldi r16,'0'
 st Y+,r16						//pair will seems like that, example for hours
 mov temp,r18					//if u try to set high hours higher than 2
 st Y,temp						//then pair will be 02, and time_set will switch on next pair
 rjmp time_set_check_max_value

 //if number is ok(lower than max range)
 time_set_first_lower_first:

 rcall time_set_get_pair_adress
 mov temp,r18					//if nubmer is ok then first number will be like pressed
 st Y,temp						//and time_set_F flag will be set as 1, for set second number
 ldi YH,high(time_set_status)
 ldi YL,low(time_set_status)
 ld temp,Y
 ori temp,(1<<time_set_F)
 st Y,temp
 rjmp return_time_set


 //if setting second number
 time_set_second_number:

 ld r16,Y
 andi r16, ~((1<<time_set_S)|(1<<time_set_F)|(1<<time_set_B))	//now r16 = pair
 ldi YH,high(time_set_ranges)
 ldi YL,low(time_set_ranges)
 add r16,r16
 inc r16
 add YL,r16
 clr r16
 adc YH,r16
 ld r16,Y		//now r16 = max range for first number of this pair

 mov temp,r18		//temp = ASCII code of number
 subi temp,48		//now temp = number

 cp temp,r16		//if nubmer > max range, then continue
 breq time_set_first_lower_second //if number is ok then go to label
 brlo time_set_first_lower_second //if number is ok then go to label

 mov temp,r16

 time_set_first_lower_second:
 ldi r16,48				//
 add temp,r16			// temp = ASCII code of number
 rcall time_set_get_pair_adress	//get to Y pair adress
 adiw YH:YL,1
 st Y,temp				//store second number to RAM
 

 time_set_check_max_value:
 //check for max value
 rcall time_set_get_pair_adress		//load to Y adress of pair
 ld temp,Y+			//temp = first number(ASCII)
 subi temp,48		//temp = first number
 ldi r16,10
 mul temp,r16
 mov r16,r0			//r16 = first number * 10
 ld temp,Y			//temp = second number(ASCII)
 subi temp,48		//temp = second number
 add r16,temp		//r16 is value of pair

 ldi YH,high(time_set_status)
 ldi YL,low(time_set_status)
 ld temp,Y
 andi temp, 0b0001_1111		//temp = number of pair
 ldi YH,high(time_set_max_values)
 ldi YL,low(time_set_max_values)
 add YL,temp
 clr temp
 adc YH,temp
 ld temp,Y					//temp = max value for current pair

 cp r16,temp				//if current value of pair > max value for this pair, then continue
 breq time_set_value_is_ok	//
 brlo time_set_value_is_ok	//else - all is ok, return

 clr r16				//r16, is a counter for first number
 time_set_div_label:
 inc r16
 subi temp,10
 brpl time_set_div_label
 dec r16				//now r16 contain first number
 push r16				//and temp contain second
 ldi r16,10
 add temp,r16
 pop r16

 push r18
 ldi r18,48				//convert numbers to ASCII symbols
 add r16,r18
 add temp,r18
 pop r18

 rcall time_set_get_pair_adress
 st Y+,r16				//save max value in time_value string
 st Y,temp

 time_set_value_is_ok:
 rcall time_set_set_buffer	//save values of current pair to buffer
 rcall time_set_next_pair	//set next pair
 
 return_time_set:
 rcall time_set_set_buffer	//save values of current pair to buffer

 pop r16
 pop YL
 pop YH
 pop temp
 ret
 //========MACRO============================================

 //------INIT-------------------------------
 .macro time_set_init
 ldi YH,high(time_set_ranges)
 ldi YL,low(time_set_ranges)

 ldi temp,2
 st Y+,temp		//max hours high 2
 ldi temp,9
 st Y+,temp		//max hours low 9

 ldi temp,6
 st Y+,temp		//max minutes high 6
 ldi temp,9
 st Y+,temp		//max minutes low 9

 ldi temp,3
 st Y+,temp		//max date high 3
 ldi temp,9
 st Y+,temp		//max date low 9

 ldi temp,1
 st Y+,temp		//max month high 1
 ldi temp,9
 st Y+,temp		//max month low 9

 st Y+,temp		//max year high 9
 st Y+,temp		//max year low 9

 ldi YH,high(time_set_max_values)
 ldi YL,low(time_set_max_values)

 ldi temp,24
 st Y+,temp

 ldi temp,60
 st Y+,temp

 ldi temp,31
 st Y+,temp

 ldi temp,12
 st Y+,temp

 ldi temp,99
 st Y+,temp

 .endm
 
 //=========================================================