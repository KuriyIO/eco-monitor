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
 //==============TIMER 1 (16-bit)============================================

 //======MACRO========================
 //------Timer 1 Init-----------
 .macro Timer1_Init
 cli
 ldi temp, 0 | (1<<OCIE1A) | (1<<OCIE1B)  //enable A Match Interrupt and Overflow Interrupt
 out TIMSK,temp 
 clr temp
 out TCCR1A,temp		//Normal Timer Mode
 ldi temp,0 | (1<<CS12)
 out TCCR1B,temp		//Set prescaler 1/256
 ldi r16,0x18			
 ldi r17,0x6A
 out OCR1AH,r16			//set A as 6250
 out OCR1AL,r17			//that mean timer tick is 0.1sec
 ldi r16,0x84			//????????????????DEBUG
 ldi r17,0x24
 out OCR1BH,r16			//set B as 62500
 out OCR1BL,r17			//that mean timer tick is 0.1sec
 clr temp
 out TCNT1H,temp		//set current timer/counter as 0
 out TCNT1L,temp
 sei
 .endm
 //-----------------------------
 //======INTERRUPTS AND PROC==========
 //------On Match A Interrupt------
 Keyboard_check_interrupt:
 push temp
 push YH
 push YL

 keyboard_check

 cli
 in YL, OCR1AL
 in YH, OCR1AH
 ldi temp,0x6A
 add YL,temp		//OCR1A += 6250
 ldi temp, 0x18
 adc YH,temp
 out OCR1AH,YH
 out OCR1AL,YL

 clr YL
 clr YH
 out TCNT1H,YH
 out TCNT1L,YL

 ldi YH,high(tick_count)
 ldi YL,low(tick_count)
 sei
 ld temp,Y
 inc temp
 st Y,temp	

 pop YL
 pop YH
 pop temp
 reti
 //--------------------------------
 //=====================================================================