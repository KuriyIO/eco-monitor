/*
 * sound.inc
 *
 *  Created: 10.11.2015 23:44:51
 *   Author: Hogs
 */ 

 .equ BUZZER = 6				//6 - is number of pin for buzzer
 .equ sound_pause_status = 1	//number of sound_status bit, for get play/pause status
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
// ldi temp, 0 | (1<<WGM01) | (1<<CS02)
// out TCCR0, temp //set timer in CTC mode with 256 prescaler (disable)
 clr temp
 out TCNT0,temp	//set counter as 0
 ldi temp,sound_freq
 out OCR0,temp		//set sound frequency
 in temp, TIMSK
 sbr temp, OCIE0
 out TIMSK, temp	//enable On match timer 0 interrupt
 .endm
 //-------Timer 0 enable--------------------------
 .macro Timer0_enable
 push temp
 ldi temp, 0 | (1<<WGM01) | (1<<CS02)
 out TCCR0, temp //set timer in CTC mode with 256 prescaler (enable)
 pop temp
 .endm
 //-------Timer 0 disable-------------------------
 .macro Timer0_disable
 push temp
 clr temp
 out TCCR0, temp //set timer 0 (disable)
 in temp, portd
 cbr temp,BUZZER
 out portd, temp	//clear buzzer's pin
 pop temp
 .endm
 //-------Sound Alarm enable----------------------
 .macro sound_alarm_enable
 push temp
 push YH
 push YL

 cli
 ldi YH,high(sound_status)	//
 ldi YL,low(sound_status)	//set sound status in RAM
 sei
 ldi temp, 0 | (1<<sound_general_status) | (1<<sound_pause_status)
 st Y,temp	//set general sound status and pause status

 in temp, TIMSK
 ori temp, 0 | (1<<OCIE1B)
 out TIMSK, temp	//enable interrupt(every 1sec)

 Timer0_enable		//enable timer0

 pop YL
 pop YH
 pop temp
 .endm
 //-------Sound Alarm disable---------------------
 .macro sound_alarm_disable
 push temp
 push YH
 push YL

 cli
 ldi YH,high(sound_status)	//
 ldi YL,low(sound_status)	//set sound status in RAM
 sei
 ldi temp, 0
 st Y,temp			//set general sound status and pause status as 0

 in temp, TIMSK
 andi temp, ~(0 | (1<<OCIE1B))
 out TIMSK, temp	//disable interrupt

 Timer0_disable		//stop sound generation

 pop YL
 pop YH
 pop temp
 .endm
 //-----------------------------------------------

 //=======Sound interrupts==================================

 //-------Sound generation interrupt-----------
 Sound_generation:
 push temp
 sbis pind, BUZZER
 rjmp Buzzer_set_1
 in temp, portd
 andi temp, ~(1<<BUZZER)	//if buzzer pin is 1, then set it as 0
 out portd,temp
 pop temp
 reti
 Buzzer_set_1:
 in temp, portd
 ori temp, (1<<BUZZER)		//if buzzer pin is 0, then set is as 1
 out portd,temp
 pop temp
 reti

 //--------Sound play/pause interrupt----------
 Sound_play_and_pause:
 push temp
 push YH
 push YL

 ldi YL,0x6A
 ldi YH,0x18
 out OCR1AH,YH
 out OCR1AL,YL

 ldi YH,high(sound_status)	//
 ldi YL,low(sound_status)	//read sound status from RAM
 ld temp,Y

 sbrc temp,sound_pause_status
 rjmp sound_pause

 timer0_enable	//enable sound generation
 andi temp, ~(1<<sound_pause_status)
 st Y,temp	//set pause status as 0

 pop YL
 pop YH
 pop temp
 reti

 sound_pause:
 timer0_disable	//disable sound generation
 ori temp, (1<<sound_pause_status)
 st Y,temp	//set pause status as 1

 pop YL
 pop YH
 pop temp
 reti
 //--------------------------------------------
 //=========================================================