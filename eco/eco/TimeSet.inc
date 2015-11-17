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

 ldi YH,high(time_set_status)
 ldi YL,low(time_set_status)
 ld r16,Y

 sbrs r16,time_set_S
 rjmp return_time_set_blink

 ldi YH,high(time_set_blink_count)
 ldi YL,low(time_set_blink_count)
 ld temp,Y

 cpi temp,10
 brlo return_time_set_blink

 sbrs r16,time_set_B
 rjmp blink_pair

 ldi YH,high(time_set_blink_count)
 ldi YL,low(time_set_blink_count)
 clr temp
 st Y,temp

 ldi YH,high(time_set_status)
 ldi YL,low(time_set_status)
 andi r16, ~(1<<time_set_B)
 st Y,r16

 rcall time_set_get_pair_adress
 ldi temp, ' '
 st Y+,temp
 st Y,temp

 rjmp return_time_set_blink

 blink_pair:

 ldi YH,high(time_set_blink_count)
 ldi YL,low(time_set_blink_count)
 clr temp
 st Y,temp

 ldi YH,high(time_set_status)
 ldi YL,low(time_set_status)
 ori r16, (1<<time_set_B)
 st Y,r16

 push ZH
 push ZL
 rcall time_set_get_pair_adress
 ldi ZH,high(time_set_buffer)
 ldi ZL,low(time_set_buffer)
 ld temp,Z+
 st Y+,temp
 ld temp,Z
 st Y,temp
 pop ZL
 pop ZH


 return_time_set_blink:
 menu_refresh

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

 ldi YH,high(time_set_status)
 ldi YL,low(time_set_status)
 ld temp,Y

 andi temp,~(1<<time_set_F)
 inc temp

 st Y,temp

 pop temp
 pop YL
 pop YH
 ret
 //----------------------------------------------

 //========MACRO============================================
 //-----------------------------------------
 .macro time_set_start
 
 ldi YH,high(time_set_status)
 ldi YL,low(time_set_status)
 ld temp,Y

 sbrs temp,time_set_S
 rjmp enable_time_set

 ldi temp, 0x00
 st Y,temp
 rcall I2C_Clock_Set_New_Time
 Refresh_time
 rjmp return_time_set_start

 enable_time_set:

 ldi temp,(1<<time_set_S)
 st Y,temp

 return_time_set_start:
 menu_refresh

 .endm
 //-----------------------------------------
 .macro time_set_set_next_pair
 
 ldi YH,high(time_set_status)
 ldi YL,low(time_set_status)
 ld temp,Y

 sbrc temp,time_set_S
 rjmp return_time_set_set_next_pair

 rcall time_set_next_pair

 return_time_set_set_next_pair:
 menu_refresh

 .endm
 //------INIT-------------------------------
 .macro time_set_init
 ldi YH,high(time_set_ranges)
 ldi YL,low(time_set_ranges)

 st Y+,2		//max hours high 2
 st Y+,9		

 st Y+,6		//max minutes high 6
 st Y+,9

 st Y+,3		//max date high 3
 st Y+,9		

 st Y+,1		//max month high 1
 st Y+,9

 st Y+,9		//
 st Y+,9

 ldi YH,high(time_set_max_values)
 ldi YL,low(time_set_max_values)

 st Y+,24
 st Y+,60
 st Y+,31
 st Y+,12
 st Y+,99

 .endm
 //-----------------------------------------


 .macro time_set
 push temp
 push YH
 push YL
 push r16

 ldi YH,high(time_set_status)
 ldi YL,low(time_set_status)
 ld temp,Y

 sbrs temp,time_set_S	//return if time set status is off(0)
 rjmp return_time_set

 //time set mode is on
 sbrs temp,time_set_F	//skip if number is second
 rjmp time_set_second_number

 //number is first
 ldi temp,@0		//temp = ASCII code of number
 subi temp,48		//now temp = number

 ld r16,Y
 andi r16, ~((1<<time_set_S)|(1<<time_set_F)|(1<<time_set_B))	//now temp = pair
 ldi YH,high(time_set_ranges)
 ldi YL,low(time_set_ranges)
 add YL,r16
 clr temp
 adc YH,r16
 ld r16,Y		//now r16 = max range for first number of this pair

 cp r16,temp
 brlo time_set_first_lower_first

 //if number is higher than max range
 rcall time_set_get_pair_adress
 ldi r16,'0'
 st Y+,r16
 ldi temp,@0
 st Y,temp
 rcall time_set_next_pair
 rjmp return_time_set

 //if number is ok(lower than max range)
 time_set_first_lower_first:

 rcall time_set_get_pair_adress
 ldi temp,@0
 st Y,temp
 ldi YH,high(time_set_status)
 ldi YL,low(time_set_status)
 ld temp,Y
 ori temp,(1<<time_set_F)
 st Y,temp
 rjmp return_time_set


 time_set_second_number:
 ldi temp,@0		//temp = ASCII code of number
 subi temp,48		//now temp = number

 ld r16,Y
 andi r16, ~((1<<time_set_S)|(1<<time_set_F)|(1<<time_set_B))	//now temp = pair
 ldi YH,high(time_set_ranges)
 ldi YL,low(time_set_ranges)
 add YL,r16
 clr temp
 adc YH,r16
 ld r16,Y		//now r16 = max range for first number of this pair

 cp r16,temp
 ldi temp,@0
 brlo time_set_first_lower_second

 mov temp,r16
 ldi r16,48
 add temp,r16

 time_set_first_lower_second:
 rcall time_set_get_pair_adress
 st Y,temp
 rcall time_set_next_pair

 return_time_set:
 rcall time_set_set_buffer

 pop r16
 pop YL
 pop YH
 pop temp
 .endm
 //=========================================================