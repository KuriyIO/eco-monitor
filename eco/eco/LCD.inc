/*
 * Macro and procedures
 * for HD44780 like LCD 
 * Sorry for bad english...
 */ 

 .equ data = 0x04	   //create constant cmd = 0b00000100 
 .equ cmd = 0		   //create constant data = 0

 //========MACRO================================================================
 
//------45 microseconds delay---------------------------------------------
  
 .macro delay_45us
 rcall delay_45us_proc
 .endm

//------2 ms delay---------------------------------------------
  
 .macro delay_2ms
 rcall delay_2ms_proc
 .endm

//------------------------------------------------------------------------

 //      LCD initialization
 .macro LCD_init
 LCD_port_init		//init port C as output

 ldi temp,0x20
 out portc,temp
 sbi portc,3
 lpm
 cbi portc,3
 ldi temp,0xF0
 out portc,temp

 LCD_send_cmd 0x28	//init 4-bit mode 2 lines
 delay_45us
 LCD_send_cmd 0x01	//clear display
 delay_2ms
 LCD_send_cmd 0x0F	//display ON
 delay_45us

 LCD_send_cmd 0b0100_0000	//Set CGRAM adress as 0x00
 delay_45us

 LCD_send_symbol 0b00000000			//Start sending bytes for draw a new symbol
 delay_45us							//in this case i draw upper arrow
 LCD_send_symbol 0b00000000			//for do that i sending next bytes
 delay_45us							//
 LCD_send_symbol 0b00000000			//00000000
 delay_45us							//00000000
 LCD_send_symbol 0b00000000			//00000000
 delay_45us							//00000000
 LCD_send_symbol 0b00000100			//00000100
 delay_45us							//00001110
 LCD_send_symbol 0b00001110			//00011111
 delay_45us							//00000000
 LCD_send_symbol 0b00011111			//
 delay_45us							//
 LCD_send_symbol 0b00000000			//
 delay_45us							//

 LCD_send_symbol 0b00000000			//the same like previous but for draw down arrow
 delay_45us
 LCD_send_symbol 0b00011111
 delay_45us
 LCD_send_symbol 0b00001110
 delay_45us
 LCD_send_symbol 0b00000100
 delay_45us
 LCD_send_symbol 0b00000000
 delay_45us
 LCD_send_symbol 0b00000000
 delay_45us
 LCD_send_symbol 0b00000000
 delay_45us
 LCD_send_symbol 0b00000000
 delay_45us

 first_line	//set DDRAM as 0 (in start of first line)
 .endm

//------------------------------------------------------------------------

 //      LCD clear
 .macro LCD_clear
 push temp
 push byte
 LCD_send_cmd 0x01	//clear display
 delay_2ms
 pop byte
 pop temp
 .endm

//------------------------------------------------------------------------

 //      1st line
 .macro first_line
 push temp
 push byte
 LCD_send_cmd 0x80	//first line
 delay_45us
 pop byte
 pop temp
 .endm

//------------------------------------------------------------------------

 //      2nd line
 .macro second_line
 push temp
 push byte
 LCD_send_cmd 0xC0	//second line
 delay_45us
 pop byte
 pop temp
 .endm

//------------------------------------------------------------------------

 //      set port C as output for working with display
 .macro LCD_port_init  
 ldi temp,0xFC
 out ddrc,temp
 clr temp
 out portc,temp
 .endm

//------------------------------------------------------------------------

 //      send command to LCD
 .macro LCD_send_cmd
 push temp
 push byte	
 ldi temp,cmd
 ldi byte,@0
 rcall LCD_send_byte_proc
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
 rcall LCD_send_byte_proc
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
 rcall LCD_send_byte_proc	//print symbol to LCD
 delay_45us
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
 rcall LCD_send_byte_proc	//print symbol to LCD
 delay_45us
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

//------45 microseconds delay---------------------------------------------
      
 delay_45us_proc:
 push r25
 ser r25 
 delay45: dec  r25
 brne delay45
 pop r25
 ret

 //------2 ms delay-------------------------------------------------------
      
 delay_2ms_proc:
 push r25
 push r24
    ldi  r24, 42
    ldi  r25, 142
delay2: dec  r25
    brne delay2
    dec  r24
    brne delay2
    nop
 pop r24
 pop r25
 ret

//------Send byte to LCD as 4-but mode------------------------------------

 LCD_send_byte_proc:
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
 sbi portc,3	//enable E signal
 lpm			//3 clocks delay
 cbi portc,3	//disable E signal
 delay_45us
 //======Low half-byte send========
 andi temp,0b0000_0100
 pop byte		//get symbol from stack
 andi byte,0x0F //clear high half-byte
				//example:
				//before andi: byte = 0b01011010
				//after: byte = 0b00001010

 lsl byte		//logical shift left
 lsl byte		//example:
 lsl byte		//00001010 <- 00010100 <- 00101000 <- 01010000 <- 10100000
 lsl byte

 or temp,byte	//add symbol's low half-byte 
 out portc,temp //out temp to port C
 lpm			//3 clocks delay
 sbi portc,3	//enable E
 lpm			//3 clocks delay
 cbi portc,3	//disable E
 lpm			//3 clocks delay
 //=====Set port C as 0xFF=========
 ldi byte,0xF0
 out portc,byte	//out to port C 0b11110000
 ret
//------------------------------------------------------------------------
//                          sorry for bad english
