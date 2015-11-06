/*
 * timers.inc
 *
 *  Created: 05.11.2015 0:04:38
 *   Author: Hogs
 */ 

 //==============TIMER 1 (16-bit)============================================

 //======MACRO========================
 //------Timer 1 Init-----------
 .macro Timer1_Init
 cli
 ldi temp, 0 | (1<<OCIE1A)  //enable A Match Interrupt
 out TIMSK,temp 
 clr temp
 out TCCR1A,temp		//Normal Timer Mode
 ldi temp,0b0000_0011
 out TCCR1B,temp		//Set prescaler 1/64
 ldi r16,0x61			
 ldi r17,0xA8
 out OCR1AH,r16			//set A as 25000
 out OCR1AL,r17			//that mean timer tick is 0.1sec
 clr temp
 out TCNT1H,temp		//set current timer/counter as 0
 out TCNT1L,temp
 sei
 .endm
 //-----------------------------
 //======INTERRUPTS AND PROC==========
 //------On Match A Interrupt------
 Keyboard_check_interrupt:
 cli
 push temp
 push YH
 push YL

 keyboard_check

 clr temp
 out TCNT1H,temp		//set current timer/counter as 0
 out TCNT1L,temp

 ldi YH,high(tick_count)
 ldi YL,low(tick_count)
 ld temp,Y
 inc temp
 st Y,temp	

 pop YL
 pop YH
 pop temp
 sei
 reti
 //--------------------------------
 //==========================================================================