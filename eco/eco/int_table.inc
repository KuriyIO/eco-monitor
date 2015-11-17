/*
 * Interrupt vectors table
 */ 

 //=========INTERRUPT VECTORS TABLE=============================================
 .org $000 //RESET
 jmp reset 
 .org $002 //INT0 External Interrupt Request 0
 reti
 .org $004 //INT1 External Interrupt Request 1
 reti
 .org $006 //INT2 External Interrupt Request 2
 reti
 .org $008 //TIMER2 COMP Timer/Counter2 Compare Match
 reti
 .org $00A //TIMER2 OVF Timer/Counter2 Overflow
 reti
 .org $00C //TIMER1 CAPT Timer/Counter1 Capture Event
 reti
 .org $00E //TIMER1 COMPA Timer/Counter1 Compare Match A
 jmp Keyboard_check_interrupt
 .org $010 //TIMER1 COMPB Timer/Counter1 Compare Match B
 rjmp Sound_play_and_pause
 .org $012 //TIMER1 OVF Timer/Counter1 Overflow
 reti
 .org $014 //TIMER0 COMP Timer/Counter0 Compare Match
 rjmp Sound_generation
 .org $016 //TIMER0 OVF Timer/Counter0 Overflow
 reti
 .org $018 //SPI, STC Serial Transfer Complete
 reti
 .org $01A //USART, RXC USART, Rx Complete
 reti
 .org $01C //USART, UDRE USART Data Register Empty
 reti
 .org $01E //USART, TXC USART, Tx Complete
 reti
 .org $020 //ADC ADC Conversion Complete
 reti
 .org $022 //EE_RDY EEPROM Ready
 reti
 .org $024 //ANA_COMP Analog Comparator
 reti
 .org $026 //TWI Two-wire Serial Interface
 rjmp I2C_Interrupt
 .org $028 //SPM_RDY Store Program Memory Ready
 reti
//===============================================================================================

