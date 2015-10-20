/*
 * Macro and procedures
 * for HD44780 like LCD 
 * Sorry for bad english...
 */ 

 .def temp=r17		   //define r16 as temp
 .def byte=r16		   //define r17 as byte
 .equ data = 0x04	   //create constant cmd = 0b00000100 
 .equ cmd = 0		   //create constant data = 0

 //========MACRO================================================================

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
 .endm

//------------------------------------------------------------------------

 //      45 microseconds delay
 .macro delay_45us
 ser r25 
 delay45: dec  r25
 brne delay45
 .endm

//------------------------------------------------------------------------

 //      1st line
 .macro first_line
 LCD_send_cmd 0x80	//first line
 delay_45us
 .endm

//------------------------------------------------------------------------

 //      2nd line
 .macro second_line
 LCD_send_cmd 0xC0	//second line
 delay_45us
 .endm

//------------------------------------------------------------------------

 //      2 ms delay
 .macro delay_2ms
    ldi  r24, 42
    ldi  r25, 142
delay2: dec  r25
    brne delay2
    dec  r24
    brne delay2
    nop
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
 ldi temp,cmd
 ldi byte,@0
 rcall LCD_send_byte_proc
 .endm

//------------------------------------------------------------------------

 //      print 16-symbol string from sram adress
 .macro LCD_print_str
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

 .endm

//------------------------------------------------------------------------

 //      Read from EEPROM and write it in SRAM
 .macro load_strings_to_SRAM
 clr YH		 //load to YH register 0
 ldi YL,0x60 //load to YL 0x60 coz 0x0060 - sram start
 clr ZL		 //clear Z register
 clr ZH
 ldi r19,97				//r19 is a counter

 EE_read_loop:			//loop 96 times
 rcall EEPROM_read_byte //read byte from EEPROM
 st Y+,byte				//Store byte to SRAM and post-Inc adress
 inc ZL					//increment Z register after reading byte

 dec r19				//loop things...
 brne EE_read_loop		//go to EE_read_loop label if r19 is not 0
 .endm

//------------------------------------------------------------------------

//==============PROCEDURES=====================================================

//------Read Byte form EEPROM---------------------------------------------

 EEPROM_read_byte:
 ; Wait for completion of previous write
 sbic EECR,EEWE
 rjmp EEPROM_read_byte

 cli //disable all interrupts

 ; Set up address (Z register) in address register
 out EEARH, ZH
 out EEARL, ZL
 ; Start eeprom read by writing EERE
 sbi EECR,EERE
 ; Read data from data register
 in byte,EEDR

 sei //enable interrupts
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
