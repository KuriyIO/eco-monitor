/*
 * UART.inc
 *
 *  Created: 26.03.2016 0:01:18
 *   Author: Hogs
 */ 

.equ baudrate = 9600
.equ bauddivider = XTAL / (16 * baudrate) - 1

.equ TX_bs = 0	//0 is TX busy flag in UART_Status register
.equ RX_bs = 0	//0 is RX busy flag in UART_Status register

//==========MACRO=========================
//-------- UART init ---------------------
.macro UART_init
/* Set baudrate */
ldi temp, low(bauddivider)
out UBRRL, temp
ldi temp, high(bauddivider)
out UBRRH, temp

/* Set interrupts */
ldi temp, (1<<RXEN)|(1<<TXEN)
out UCSRB, temp	//RX and TX lines - enable, interrupts disable

/* Set else */
clr temp
out UCSRA, temp

ldi temp, (1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0)
out UCSRC, temp

.endm
//----------------------------------------

//------- Clear TX buffer ----------------
.macro UART_clear_TX_buffer
push YH
push YL
push temp
push byte

/* Clear buffer pointer */
ldi YH, high(UART_TX_byte_p)
ldi YL, low(UART_TX_byte_p)
clr temp	//temp = 0
st Y, temp

/* Clear buffer */
ldi YH, high(UART_TX_buffer)
ldi YL, low(UART_TX_buffer)	//load buffer adress

ser temp	//temp = 0xFF
clr byte	//byte = 0

UART_clear_TX_loop:
st Y+, byte	//store 0
dec temp
brne UART_clear_TX_loop

pop byte
pop temp
pop YL
pop YH
.endm
//----------------------------------------

//------- Clear RX buffer ----------------
.macro UART_clear_RX_buffer
push YH
push YL
push temp
push byte

/* Clear buffer pointer */
ldi YH, high(UART_RX_byte_p)
ldi YL, low(UART_RX_byte_p)
clr temp	//temp = 0
st Y, temp

/* Clear buffer */
ldi YH, high(UART_RX_buffer)
ldi YL, low(UART_RX_buffer)	//load buffer adress

ser temp	//temp = 0xFF
clr byte	//byte = 0

UART_clear_RX_loop:
st Y+, byte	//store 0
dec temp
brne UART_clear_RX_loop

pop byte
pop temp
pop YL
pop YH
.endm
//----------------------------------------

//------ Start transmit ------------------
.macro UART_start_transmit
push YH
push YL
push byte
push temp

/* Set TX_bs flag */
ldi YH, high(UART_Status)
ldi YL, low(UART_Status)
ld temp, Y
ori temp, (1<<TX_bs)
st Y, temp

/* Set buffer pointer */
ldi YH, high(UART_TX_byte_p)
ldi YL, low(UART_TX_byte_p)
ldi temp, 1
st Y, temp	//set pointer as 1

/* Load first byte */
ldi YH, high(UART_TX_buffer)
ldi YL, low(UART_TX_buffer)
ld byte, Y

/* Send first byte and enable interrupts */
in temp, UCSRB
ori temp, (1<<UDRIE)|(1<<TXCIE)
out UDR, byte
out UCSRB, temp

pop temp
pop byte
pop YL
pop YH
.endm
//----------------------------------------

//------ Start receive ------------------
.macro UART_start_receive
push YH
push YL
push temp

/* Set RX_bs flag */
ldi YH, high(UART_Status)
ldi YL, low(UART_Status)
ld temp, Y
ori temp, (1<<RX_bs)
st Y, temp

/* Enable interrupts */
in temp, UCSRB
ori temp, (1<<RXCIE)
out UCSRB, temp

pop temp
pop YL
pop YH
.endm
//----------------------------------------

//------ Stop receive ------------------
.macro UART_stop_receive
push YH
push YL
push temp

/* Disable interrupts */
in temp, UCSRB
andi temp, ~(1<<RXCIE)
out UCSRB, temp

/* Clear RX_bs flag */
ldi YH, high(UART_Status)
ldi YL, low(UART_Status)
ld temp, Y
andi temp, ~(1<<RX_bs)
st Y, temp

pop temp
pop YL
pop YH
.endm
//----------------------------------------
//========================================

//=============INTERRUPTS=================
//-------- UDR empty interrupt -----------
UART_transmit:
push YH
push YL
push temp
push byte

/* read buffer pointer */
ldi YH, high(UART_TX_byte_p)
ldi YL, low(UART_TX_byte_p)
ld temp, Y

/* read byte from buffer */
ldi YH, high(UART_TX_buffer)
ldi YL, low(UART_TX_buffer)	//load buffer adress

add YL,temp
clr temp		//add offset
adc YH,temp

ld byte, Y

/* check for end */
cpi byte, 0
breq UART_stop_transmit	//return if end of string

out UDR, byte	//send byte

/* inc pointer */
ldi YH, high(UART_TX_byte_p)
ldi YL, low(UART_TX_byte_p)
ld temp, Y
inc temp
st Y, temp
rjmp UART_return_transmit

/* Stop transmit */
UART_stop_transmit:

/* Disable interrupts */
in temp, UCSRB
andi temp, ~((1<<UDRIE)|(1<<TXCIE))
out UCSRB, temp

/* Clear TX_bs flag */
ldi YH, high(UART_Status)
ldi YL, low(UART_Status)
ld temp, Y
andi temp, ~(1<<TX_bs)
st Y, temp

/* return */
UART_return_transmit:

pop byte
pop temp
pop YL
pop YH
reti
//----------------------------------------

//------- End transmit (TX interrupt) ----
/*UART_end_transmit:
push YH
push YL
push temp

// Disable interrupts
in temp, UCSRB
andi temp, ~((1<<UDRIE)|(1<<TXCIE))
out UCSRB, temp

// Clear TX_bs flag
ldi YH, high(UART_Status)
ldi YL, low(UART_Status)
ld temp, Y
andi temp, ~(1<<TX_bs)
st Y, temp

pop temp
pop YL
pop YH
reti*/
//----------------------------------------

//------- Receive (RX interrupt) ---------
UART_receive:
push YH
push YL
push temp
push byte

in byte, UDR	//read received byte

/* read buffer pointer */
ldi YH, high(UART_RX_byte_p)
ldi YL, low(UART_RX_byte_p)
ld temp, Y

/* Store byte to buffer */
ldi YH, high(UART_RX_buffer)
ldi YL, low(UART_RX_buffer)	//load buffer adress

add YL, temp
clr temp		//add offset
adc YH, temp

st Y, byte	//store byte

/* inc pointer */
ldi YH, high(UART_RX_byte_p)
ldi YL, low(UART_RX_byte_p)
ld temp, Y
inc temp
st Y, temp

/* return */
pop byte
pop temp
pop YL
pop YH
reti
//----------------------------------------
//========================================