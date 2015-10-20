/*
 * Interrupt vectors table
 */ 

 //=========INTERRUPT VECTORS TABLE=============================================
 .ORG 0x000        ; (RESET) 
 RJMP reset
 .ORG 0x002
 RETI             ; (INT0) External Interrupt Request 0
 .ORG 0x004
 RETI             ; (INT1) External Interrupt Request 1
 .ORG 0x006
 RETI			  ; (TIMER2 COMP) Timer/Counter2 Compare Match
 .ORG 0x008
 RETI             ; (TIMER2 OVF) Timer/Counter2 Overflow
 .ORG 0x00A
 RETI			  ; (TIMER1 CAPT) Timer/Counter1 Capture Event
 .ORG 0x00C 
 RETI             ; (TIMER1 COMPA) Timer/Counter1 Compare Match A
 .ORG 0x00E
 RETI             ; (TIMER1 COMPB) Timer/Counter1 Compare Match B
 .ORG 0x010
 RETI             ; (TIMER1 OVF) Timer/Counter1 Overflow
 .ORG 0x012
 RETI             ; (TIMER0 OVF) Timer/Counter0 Overflow
 .ORG 0x014
 RETI             ; (SPI,STC) Serial Transfer Complete
 .ORG 0x016
 RETI    	      ; (USART,RXC) USART, Rx Complete
 .ORG 0x018
 RETI             ; (USART,UDRE) USART Data Register Empty
 .ORG 0x01A
 RETI             ; (USART,TXC) USART, Tx Complete
 .ORG 0x01C
 RETI	          ; (ADC) ADC Conversion Complete
 .ORG 0x01E
 RETI             ; (EE_RDY) EEPROM Ready
 .ORG 0x020
 RETI             ; (ANA_COMP) Analog Comparator
 .ORG 0x022
 RETI             ; (TWI) 2-wire Serial Interface
 .ORG 0x024
 RETI             ; (INT2) External Interrupt Request 2
 .ORG 0x026
 RETI             ; (TIMER0 COMP) Timer/Counter0 Compare Match
 .ORG 0x028
 RETI             ; (SPM_RDY) Store Program Memory Ready
 .ORG 0x030        ; End of table
//===============================================================================================

