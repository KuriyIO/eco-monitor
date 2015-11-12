/*
 * sound.inc
 *
 *  Created: 10.11.2015 23:44:51
 *   Author: Hogs
 */ 

 .equ BUZZER = 6				//6 - is number of pin for buzzer
 .equ sound_general_status = 0	//number of sound_status bit, 
								//for get general sound status (sound_on/sound_off)
 .equ sound_freq = 23			//frequency of sound

 //=======Sound macro=======================================
 //-------Sound Init---------------------------
 .macro sound_init
 in temp, ddrd
 ori temp, (0 | (1<<BUZZER))	//set buzzer pin as output
 out ddrd,temp
 in temp, portd
 andi temp, ~(1<<BUZZER)	//set buzzer pin as 0
 out portd, temp

								//setting timer0
 clr temp
 out TCNT0,temp		//set counter as 0
 ldi temp,sound_freq
 out OCR0,temp		//set sound frequency
 in temp, TIMSK
 ori temp, (1<<OCIE0)	//
 out TIMSK, temp	//enable On match timer 0 interrupt
 .endm
 //-------Timer0 enable---------------------------
 .macro Timer0_enable
 push temp
 ldi temp, 0 | (1<<WGM01) | (1<<CS02)
 out TCCR0, temp //set timer in CTC mode with 256 prescaler (enable)
 pop temp
 .endm
 //-------Timer0 disable---------------------------
 .macro Timer0_disable
 push temp
 clr temp
 out TCCR0, temp //disable timer0
 pop temp
 .endm
  //-------Timer 0 toggle--------------------------
 .macro Timer0_toggle
 push temp

 in temp, TCCR0			//if TCCR0 is empty
 cpi temp,0				//then set timer on
 breq set_timer_on		//else set timer off

 Timer0_disable
 rjmp end_timer_toggle	//disable timer

 set_timer_on:
 Timer0_enable

 end_timer_toggle:
 pop temp
 .endm
 //=======PROCEDURES=======================================================
 //-------Sound Alarm enable----------------------
 sound_alarm_enable:
 push temp
 push YH
 push YL

 ldi YH,high(sound_status)	//
 ldi YL,low(sound_status)	//set sound status in RAM
 
 ldi temp, 0 | (1<<sound_general_status)
 st Y,temp	//set general sound status and pause status

 in temp, TIMSK
 ori temp, (1<<OCIE1B)	//set OCIE1B as 1
 out TIMSK, temp	//enable interrupt(every 0.5sec)

 Timer0_enable		//enable timer0

 pop YL
 pop YH
 pop temp
 ret
 //-------Sound Alarm disable---------------------
 sound_alarm_disable:
 push temp
 push YH
 push YL
 
 ldi YH,high(sound_status)	//
 ldi YL,low(sound_status)	//set sound status in RAM

 ldi temp, 0
 st Y,temp			//set general sound status as 0 (alarm disable:)

 in temp, TIMSK
 andi temp, ~(1<<OCIE1B)	//set OCIE1B as 0
 out TIMSK, temp	//disable interrupt

 Timer0_disable		//stop sound generation

 clr YL
 clr YH
 cli
 out TCNT1H,YH	//clear counter of timer1
 out TCNT1L,YL
 sei

 ldi r16,0x0C			//set A as standart value	
 ldi r17,0x35
 cli
 out OCR1AH,r16			//set A as 3125
 out OCR1AL,r17			//that mean timer tick is every 0.05sec
 sei

 pop YL
 pop YH
 pop temp
 ret
 //-----------------------------------------------

 //=======Sound interrupts==================================

 //-------Sound generation interrupt-----------
 Sound_generation:
 sbis pind, BUZZER
 rjmp Buzzer_set_1
 cbi portd, BUZZER	//if buzzer pin is 1, then set it as 0
 reti
 Buzzer_set_1:
 sbi portd, BUZZER	//if buzzer pin is 0, then set is as 1
 reti

 //--------Sound play/pause interrupt----------
 Sound_play_and_pause:
 push temp
 push YH
 push YL

 ldi YH,0x0C
 ldi YL,0x35
 cli
 out OCR1AH,YH	//set timer1 A as 3125
 out OCR1AL,YL	//for correct work with keyboard
 sei

 Timer0_toggle

 clr YL
 clr YH
 cli
 out TCNT1H,YH	//clear counter of timer1
 out TCNT1L,YL
 sei

 pop YL
 pop YH
 pop temp
 reti
 //--------------------------------------------
 //=========================================================