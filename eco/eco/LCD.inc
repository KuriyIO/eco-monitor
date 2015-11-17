/*
 * Macro and procedures
 * for HD44780 like LCD 
 * Sorry for bad english...
 */ 

 .equ data = 0x04	   //create constant cmd = 0b00000100 
 .equ cmd = 0		   //create constant data = 0
 .equ RS = 2		   //RS - 2nd pin in port C
 .equ E = 3			   //E - is strob line, is a 3rd pin in port C
 .equ D4 = 4		   //LCD data pins in port C
 .equ D5 = 5
 .equ D6 = 6
 .equ D7 = 7

 //========MACRO================================================================
 
//----------delay---------------------------------------------
  
 .macro delay
 push r25
 push r24
 push r23

 ldi r25,@0
 ldi r24,@1
 ldi r23,@2

 call delay_proc

 pop r23
 pop r24
 pop r25
 .endm

//------------------------------------------------------------------------

 //      LCD initialization
 .macro LCD_init
 LCD_port_init		//init port C as output

 delay 242,15,5		//50 ms delay
 ldi temp,0x30
 out portc,temp		//send 0x30
 sbi portc,E
 lpm
 cbi portc,E
 delay 229,30,9		//100 ms delay
 sbi portc,E
 lpm				//send 0x30
 cbi portc,E
 delay 242,15,5		//50 ms delay 
 sbi portc,E
 lpm				//send 0x30
 cbi portc,E
 delay 242,15,5		//50 ms delay
 ldi temp,0x20
 out portc,temp
 sbi portc,E		//send 0x20
 lpm
 cbi portc,E
 delay 240,1,1		//delay 45us

 LCD_send_cmd 0x28	//init 4-bit mode 2 lines
 delay 240,1,1 //delay 45us
 LCD_send_cmd 0x01	//clear display
 delay 142,42,1 //delay 2ms
 LCD_send_cmd 0x0F	//display ON
 delay 142,42,1 //delay 2ms

 LCD_send_cmd 0b0100_0000	//Set CGRAM adress as 0x00
 delay 240,1,1 //delay 45us

 LCD_send_symbol 0b00000000			//Start sending bytes for draw a new symbol
 delay 240,1,1 //delay 45us			//in this case i draw upper arrow
 LCD_send_symbol 0b00000000			//for do that i sending next bytes
 delay 240,1,1 //delay 45us			//
 LCD_send_symbol 0b00000000			//00000000
 delay 240,1,1 //delay 45us			//00000000
 LCD_send_symbol 0b00000000			//00000000
 delay 240,1,1 //delay 45us			//00000000
 LCD_send_symbol 0b00000100			//00000100
 delay 240,1,1 //delay 45us			//00001110
 LCD_send_symbol 0b00001110			//00011111
 delay 240,1,1 //delay 45us			//00000000
 LCD_send_symbol 0b00011111			//
 delay 240,1,1 //delay 45us			//
 LCD_send_symbol 0b00000000			//
 delay 240,1,1 //delay 45us			//

 LCD_send_symbol 0b00000000			//the same like previous but for draw down arrow
 delay 240,1,1 //delay 45us
 LCD_send_symbol 0b00011111
 delay 240,1,1 //delay 45us
 LCD_send_symbol 0b00001110
 delay 240,1,1 //delay 45us
 LCD_send_symbol 0b00000100
 delay 240,1,1 //delay 45us
 LCD_send_symbol 0b00000000
 delay 240,1,1 //delay 45us
 LCD_send_symbol 0b00000000
 delay 240,1,1 //delay 45us
 LCD_send_symbol 0b00000000
 delay 240,1,1 //delay 45us
 LCD_send_symbol 0b00000000
 delay 240,1,1 //delay 45us

 first_line	//set DDRAM as 0 (in start of first line)
 .endm

//------------------------------------------------------------------------

 //      LCD clear
 .macro LCD_clear
 push temp
 push byte
 LCD_send_cmd 0x01	//clear display
 delay 142,42,1 //delay 2ms
 pop byte
 pop temp
 .endm

//------------------------------------------------------------------------

 //      1st line
 .macro first_line
 push temp
 push byte
 LCD_send_cmd 0x80	//first line
 delay 240,1,1 //delay 45us
 pop byte
 pop temp
 .endm

//------------------------------------------------------------------------

 //      2nd line
 .macro second_line
 push temp
 push byte
 LCD_send_cmd 0xC0	//second line
 delay 240,1,1 //delay 45us
 pop byte
 pop temp
 .endm

//------------------------------------------------------------------------

 //      set port C as output for working with display
 .macro LCD_port_init  
 in temp, ddrc
 ori temp, (0 | (1<<RS) | (1<<E) | (1<<D4) | (1<<D5) | (1<<D6) | (1<<D7))
 out ddrc,temp
 in temp,portc
 andi temp, (~(0 | (1<<RS) | (1<<E) | (1<<D4) | (1<<D5) | (1<<D6) | (1<<D7)))
 out portc,temp
 .endm

//------------------------------------------------------------------------

 //      send command to LCD
 .macro LCD_send_cmd
 push temp
 push byte	
 ldi temp,cmd
 ldi byte,@0
 call LCD_send_byte_proc
 pop byte
 pop temp
 .endm

//------------------------------------------------------------------------

 //      send symbol to LCD
 .macro LCD_send_symbol	
 push temp
 push byte
 ldi temp,4
 ldi byte,@0
 call LCD_send_byte_proc
 pop byte
 pop temp
 .endm

//------------------------------------------------------------------------

 //      print 16-symbol string from sram adress (if adress is const)
 .macro LCD_print_str
 push ZH
 push ZL
 push temp
 push byte
 push r19
 ldi ZH,high(@0)	//load to high Z-byte high byte of adress
 ldi ZL,low(@0)	//load to low Z-byte low byte of adress
 ldi temp,data	//temp = 0x04 for send data to LCD

 ldi r19,17			//r19 is a counter
 sram_read_loop:	//loop 16 times
 ld byte,Z+			//read from SRAM and post-inc
 call LCD_send_byte_proc	//print symbol to LCD
 delay 240,1,1 //delay 45us
 dec r19			//loop things...
 brne sram_read_loop//go to sram_read_loop label if r19 is not 0

 pop r19
 pop byte
 pop temp
 pop ZL
 pop ZH
 .endm

//------------------------------------------------------------------------

 //      print 16-symbol string from sram adress (if adress is in register)
 //					ADRESS MUST BE IN X REGISTER
 .macro LCD_print_str_reg
 push temp
 push byte
 push XL
 push XH
 push r19

 ldi temp,data	//temp = 0x04 for send data to LCD

 ldi r19,17			//r19 is a counter
 sram_read_loop:	//loop 16 times
 ld byte,X+			//read from SRAM and post-inc
 call LCD_send_byte_proc	//print symbol to LCD
 delay 240,1,1 //delay 45us
 dec r19			//loop things...
 brne sram_read_loop//go to sram_read_loop label if r19 is not 0

 pop r19
 pop XH
 pop XL
 pop byte
 pop temp
 .endm

//------------------------------------------------------------------------

//==============PROCEDURES=====================================================

 //------delay proc-------------------------------------------------------
      
 delay_proc:

delay_proc_label: 
	dec  r25
    brne delay_proc_label
    dec  r24
    brne delay_proc_label
	dec  r23
    brne delay_proc_label

 ret

//------Send byte to LCD as 4-but mode------------------------------------

 LCD_send_byte_proc:
 cli
 push byte //save symbol to stack
 andi temp,0b0000_0100
 out portc,temp //setting RS line

 //======High half-byte send=======
 andi byte,0xF0 //clear low half-byte
				//example:
				//before ANDI byte was 0b01011111
				//after byte is 0b01010000

 or temp,byte	//add high half-byte to temp register
				//example:
				//before: temp = 0b00000100, byte = 0b11110000
				//after: temp = 0b11110100

 out portc,temp //out temp to port C
 lpm			//3 clocks delay
 sbi portc,E	//enable E signal
 lpm			//3 clocks delay
 cbi portc,E	//disable E signal
 delay 240,1,1 //delay 45us
 //======Low half-byte send========
 andi temp,0b0000_0100
 pop byte		//get symbol from stack
 swap byte		//swap byte register
				//example:
				//before swap byte was: 0b01011010
				//after swap byte is:   0b10100101
 andi byte,0xF0 //clear high half-byte
				//example:
				//before andi: byte = 0b01011010
				//after: byte = 0b01010000

 or temp,byte	//add symbol's low half-byte 
 out portc,temp //out temp to port C
 lpm			//3 clocks delay
 sbi portc,E	//enable E
 lpm			//3 clocks delay
 cbi portc,E	//disable E
 lpm			//3 clocks delay
 //=====Set port C as 0xFF=========
 ldi byte,0xF0
 out portc,byte	//out to port C 0b11110000
 sei
 ret
//------------------------------------------------------------------------
//                          sorry for bad english
