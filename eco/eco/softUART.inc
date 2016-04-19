/*
 * softUART.inc
 *
 *  Created: 20.02.2016 1:13:40
 *   Author: Hogs
 */ 

//=============DEFINES==========================
.def bit = r19
.def pins = r20
.equ TX_pin = 0
.equ RX_pin = 1
.equ TX_bs = 0
.equ RX_bs = 1
//==============================================

//==========INITIALIZATION======================
//--------software UART init--------------------
.macro sUART_init
push temp
push YH
push YL

//------pins init--------
in temp, ddra
andi temp, (~(0 | (1<<RX_pin)))
ori temp, (1<<TX_pin);
out ddra, temp			//set RX_pin as input and TX_pin as output

in temp,porta
ori temp, (0 | (1<<RX_pin) | (1<<TX_pin))
out porta,temp			//enable pull-ups for RX_pin and set TX_pin as 1

//------Timer 2 init-----
ldi temp, (1<<CS21)
out tccr2,temp
ldi temp, 205
out ocr2,temp
in temp,timsk
ori temp, (1<<OCIE2)
out timsk,temp
//----------------------

//---Clear sUART registers-----
ldi YH,high(sUART_TX_byte_p)
ldi YL,low(sUART_TX_byte_p)
clr temp
st Y+, temp
st Y+, temp
st Y+, temp
st Y+, temp
st Y, temp
//-----------------------------

pop YL
pop YH
pop temp
.endm
//----------------------------------------------
//==============================================

//=============MACRO============================

//--------Send Start bit------------------------
.macro sUART_send_start
push temp

in temp,porta
andi temp, ~(1<<TX_pin)
out porta,temp

pop temp
.endm
//----------------------------------------------

//---------Send Stop bit------------------------
.macro sUART_send_stop
push temp

in temp,porta
ori temp, (1<<TX_pin)
out porta,temp

pop temp
.endm
//----------------------------------------------

//----------Start Transmit----------------------
.macro sUART_start_transmit
push YL
push YH
push temp

ldi YH,high(sUART_Status)
ldi YL,low(sUART_Status)
ld temp, Y

ori temp, (1<<TX_bs)
st Y,temp

pop temp
pop YH
pop YL
.endm
//----------------------------------------------

//---------Read current TX byte-----------------
.macro sUART_read_TX_byte
push YH
push YL
push temp

ldi YH,high(sUART_TX_byte_p)
ldi YL,low(sUART_TX_byte_p)
ld temp, Y

ldi YH,high(sUART_TX_buffer)
ldi YL,low(sUART_TX_buffer)
add YL,temp
clr temp
adc YH,temp

ld byte, Y

pop temp
pop YL
pop YH
.endm
//----------------------------------------------

//---------Read current TX_bs-------------------
.macro sUART_read_TX_bs
push YH
push YL

ldi YH,high(sUART_Status)
ldi YL,low(sUART_Status)
ld byte, Y

andi byte, (1<<TX_bs)

pop YL
pop YH
.endm
//----------------------------------------------

//---------Clear TX_bs-------------------
.macro sUART_clear_TX_bs
push YH
push YL
push temp

ldi YH,high(sUART_Status)
ldi YL,low(sUART_Status)
ld temp, Y

andi temp, ~(1<<TX_bs)
st Y, temp

pop temp
pop YL
pop YH

.endm
//----------------------------------------------

//----------Read TX bit-------------------------
.macro sUART_read_TX_bit
push YH
push YL

ldi YH,high(sUART_TX_Status)
ldi YL,low(sUART_TX_Status)
ld bit, Y

andi bit, 0b0000_1111	//clear all except TX_bit value

pop YL
pop YH
.endm
//----------------------------------------------

//----------Inc TX bit-------------------------
.macro sUART_inc_TX_bit
push YH
push YL
push bit

ldi YH,high(sUART_TX_Status)
ldi YL,low(sUART_TX_Status)
ld bit, Y
inc bit
st Y,bit

pop bit
pop YL
pop YH
.endm
//----------------------------------------------

//----------Clear TX bit------------------------
.macro sUART_clr_TX_bit
push YH
push YL
push bit

ldi YH,high(sUART_TX_Status)
ldi YL,low(sUART_TX_Status)
clr bit
st Y,bit

pop bit
pop YL
pop YH
.endm
//----------------------------------------------

//----------Clear TX_byte pointer---------------
.macro sUART_clr_TX_byte_p
push YH
push YL
push byte

ldi YH,high(sUART_TX_byte_p)
ldi YL,low(sUART_TX_byte_p)
clr byte
st Y,byte

pop byte
pop YL
pop YH
.endm
//----------------------------------------------

//----------Inc TX_byte pointer-----------------
.macro sUART_inc_TX_byte_p
push YH
push YL
push byte

ldi YH,high(sUART_TX_byte_p)
ldi YL,low(sUART_TX_byte_p)
ld byte, Y
inc byte
st Y,byte

pop byte
pop YL
pop YH
.endm
//----------------------------------------------

//---------Read current RX_bs-------------------
.macro sUART_read_RX_bs
push YH
push YL

ldi YH,high(sUART_Status)
ldi YL,low(sUART_Status)
ld byte, Y

andi byte, (1<<RX_bs)

pop YL
pop YH
.endm
//----------------------------------------------

//---------Set RX_bs as 1-------------------
.macro sUART_set_RX_bs
push YH
push YL
push temp

ldi YH,high(sUART_Status)
ldi YL,low(sUART_Status)
ld temp, Y
ori temp, (1<<RX_bs)
st Y,temp

pop temp
pop YL
pop YH
.endm
//----------------------------------------------

//---------Set RX_bs as 0-------------------
.macro sUART_clr_RX_bs
push YH
push YL
push temp

ldi YH,high(sUART_Status)
ldi YL,low(sUART_Status)
ld temp, Y
andi temp, ~(1<<RX_bs)
st Y,temp

pop temp
pop YL
pop YH
.endm
//----------------------------------------------

//----------Read RX bit-------------------------
.macro sUART_read_RX_bit
push YH
push YL

ldi YH,high(sUART_RX_Status)
ldi YL,low(sUART_RX_Status)
ld bit, Y

andi bit, 0b0000_1111	//clear all except RX_bit value

pop YL
pop YH
.endm
//----------------------------------------------

//----------Clear RX bit------------------------
.macro sUART_clr_RX_bit
push YH
push YL
push bit

ldi YH,high(sUART_RX_Status)
ldi YL,low(sUART_RX_Status)
clr bit
st Y,bit

pop bit
pop YL
pop YH
.endm
//----------------------------------------------

//----------Inc RX bit-------------------------
.macro sUART_inc_RX_bit
push YH
push YL
push bit

ldi YH,high(sUART_RX_Status)
ldi YL,low(sUART_RX_Status)
ld bit, Y
inc bit
st Y,bit

pop bit
pop YL
pop YH
.endm
//----------------------------------------------

//----------Inc RX_byte pointer-----------------
.macro sUART_inc_RX_byte_p
push YH
push YL
push byte

ldi YH,high(sUART_RX_byte_p)
ldi YL,low(sUART_RX_byte_p)
ld byte, Y
inc byte
st Y,byte

pop byte
pop YL
pop YH
.endm
//----------------------------------------------

//----------Read RX_byte------------------------
.macro sUART_read_RX_byte
push YH
push YL
push temp

ldi YH,high(sUART_RX_byte_p)
ldi YL,low(sUART_RX_byte_p)
ld temp, Y

ldi YH,high(sUART_RX_buffer)
ldi YL,low(sUART_RX_buffer)
add YL, temp
clr temp
adc YH, temp
ld byte, Y

pop temp
pop YL
pop YH
.endm
//----------------------------------------------

//----------Store RX_byte------------------------
.macro sUART_store_RX_byte
push YH
push YL
push temp

ldi YH,high(sUART_RX_byte_p)
ldi YL,low(sUART_RX_byte_p)
ld temp, Y

ldi YH,high(sUART_RX_buffer)
ldi YL,low(sUART_RX_buffer)
add YL, temp
clr temp
adc YH, temp
st Y, @0

pop temp
pop YL
pop YH
.endm
//----------------------------------------------

//==============================================

//==========INTERRUPT===========================

//----------TIMER 2 interrupt-------------------
sUART:
push byte
push bit
push temp
push pins

//- - - - - - reset timer - - - - - - - - - -
clr temp
out tcnt2, temp
//- - - - - - - - - - - - - - - - - - - - - -

//- - - - - - transmit - - - - - - - - - - -
sUART_read_TX_bs
cpi byte,0
brne PC+2
rjmp end_transmit			//if TX_bs is 0 skip transmit

sUART_read_TX_byte
cpi byte,0
brne PC+2
rjmp clear_TX				//if byte = 0(end of string) then end transmit

sUART_read_TX_bit
cpi bit,0
brne TX_last_bit_check

//---//---//---//---//---//
//    bit = 0 (start bit)

sUART_send_start
sUART_inc_TX_bit
rjmp end_transmit

//---//---//---//---//---//

TX_last_bit_check:			//check last bit (if(TX_bit==9))
cpi bit,9
brne Transmit_bit

//---//---//---//---//---//
//    bit = 9 (stop bit)

sUART_send_stop			//send stop bit
sUART_clr_TX_bit		//bit = 0
sUART_inc_TX_byte_p		//next byte
rjmp end_transmit

//---//---//---//---//---//

Transmit_bit:

//---//---//---//---//---//
//  bit>0 && bit<9 (transmit data)

dec bit
breq end_TX_shift_right		//byte>>(bit-1)
lsr byte					//C example:
dec bit						//bit--;
rjmp PC-3					//while(bit!=0)
end_TX_shift_right:			//{	byte>>1; bit--;}

andi byte, 1	//clear all bits except first
in temp, porta
andi temp, (~(1<<TX_pin))
or temp,byte
out porta,temp	//set TX_pin

sUART_inc_TX_bit	//bit++
rjmp end_transmit

//---//---//---//---//---//
clear_TX:

sUART_clear_TX_bs			//set TX_bs 0 (transmit completed)
sUART_clr_TX_byte_p			//set byte pointer 0

end_transmit:			//end of transmit block
in pins,pina
//- - - - - - - - - - - - - - - - - - - - - -

//- - - - - - receive - - - - - - - - - - - -
sUART_read_RX_bs
cpi byte,0				//if RX_bs is 0 then wait for start flag
brne PC+2
rjmp RX_wait_receive

sUART_read_RX_bit
cpi bit,8
brne RX_receive_bit

//---//---//---//---//---//
//  bit = 9 (stop bit)

sUART_clr_RX_bit	//RX_bit = 0
sUART_clr_RX_bs		//RX_bs = 0 (false)
sUART_inc_RX_byte_p	//Receive pointer ++
rjmp end_receive

//---//---//---//---//---//

RX_receive_bit:

//---//---//---//---//---//
// bit>0 && bit<9 (receive data)
andi pins, (1<<RX_pin)
lsr pins

dec bit
breq end_RX_shift_left	//while(bit !=0)
lsl pins				//{
dec bit					//	pins<<1;
rjmp PC-3				//	bit--;
end_RX_shift_left:		//}

sUART_read_RX_byte			//read current byte
or byte,pins				//add a just received bit
sUART_store_RX_byte byte	//store back current byte

sUART_inc_RX_bit			//bit++
rjmp end_receive
//---//---//---//---//---//

RX_wait_receive:
							//waiting for RX_pin = 0
andi pins, (1<<RX_pin)		//if(RX_pin == 0)
brne end_receive			//{ RX_bs = 1(true) and receive started
sUART_set_RX_bs				//	bit++; }
sUART_inc_RX_bit			//else go to end of receive
end_receive:

//- - - - - - - - - - - - - - - - - - - - - -

pop pins
pop temp
pop bit
pop byte
reti
//----------------------------------------------

//==============================================