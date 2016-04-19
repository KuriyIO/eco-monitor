/*
 * timers.inc
 *
 *  Created: 05.11.2015 0:04:38
 *   Author: Hogs
 */ 

 //==============TIMER 0 (8bit)==============================================

		//Timer 0 is reserved for sound generation
		//you can see how it works in sound.inc
		//all code which using timer 0 is there

 //==========================================================================

 //==============TIMER 2 (8bit)==============================================

		//Timer 2 is reserved for software UART
		//you can see how it works in softUART.inc
		//all code which using timer 0 is there

 //==========================================================================

 //==============TIMER 1 (16-bit)============================================

 //======MACRO========================
 //------Timer 1 Init-----------
 .macro Timer1_Init
 cli
 ldi temp, 0 | (1<<OCIE1A) //enable A Match Interrupt and Overflow Interrupt
 out TIMSK,temp 
 clr temp
 out TCCR1A,temp		//Normal Timer Mode
 ldi temp,0 | (1<<CS12)
 out TCCR1B,temp		//Set prescaler 1/256
 ldi r16,0x0C			
 ldi r17,0x35
 out OCR1AH,r16			//set A as 3125
 out OCR1AL,r17			//that mean timer tick is every 0.05sec
 ldi r16,0x7A			
 ldi r17,0x12
 out OCR1BH,r16			//set B as 32500
 out OCR1BL,r17			//that mean timer tick is every 0.5sec
 clr temp
 out TCNT1H,temp		//set current timer/counter as 0
 out TCNT1L,temp
 sei
 .endm
 //-----------------------------
 //======INTERRUPTS AND PROC==========
 //------On Match A Interrupt------
 Keyboard_check_interrupt:
 sei
 push temp
 push YH
 push YL
 

 ldi YH,high(time_set_blink_count)
 ldi YL,low(time_set_blink_count)
 ld temp,Y
 inc temp
 st Y,temp

 call time_set_blink

 keyboard_check

 ldi YH,high(sound_status)	//
 ldi YL,low(sound_status)	//get sound status in RAM
 ld temp,Y					
 sbrs temp, sound_general_status	//if sound is off then 
 rjmp skip_A_addition				//OCIE1B interrupt will never happen
									//for prevent uncorrect timer work
									//I skip (OCR1A += 3125) if sound is off

 in YL, OCR1AL
 in YH, OCR1AH
 ldi temp,0x35
 add YL,temp		//OCR1A += 3125
 ldi temp, 0x0C		//this need for correct work with keyboard
 adc YH,temp		//if sound enable
 cli
 out OCR1AH,YH
 out OCR1AL,YL
 sei
 rjmp continue_A_add	//jump over TCNT1 clear, because if sound is on
						//TCNT1 must be cleared in OCIE1B interrupt

 skip_A_addition:

 clr YL
 clr YH
 cli
 out TCNT1H,YH	//clear counter of timer1
 out TCNT1L,YL
 sei

 continue_A_add:
 ldi YH,high(tick_count)
 ldi YL,low(tick_count)
 ld temp,Y				//tick_count++
 inc temp
 st Y,temp	

 
 pop YL
 pop YH
 pop temp
 reti
 //--------------------------------
 //=====================================================================