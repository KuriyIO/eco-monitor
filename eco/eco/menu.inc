/*
 * menu.inc
 *
 *  Created: 26.10.2015 0:27:02
 *   Author: Hogs
 */ 

 //========MACRO==========================================

 //--------Init menu page--------------------
 .macro menu_init

 load_strings_to_SRAM		//save strings from eeprom to ram

 ldi YH,high(menu_location) //set current menu page as 0 (first page)
 ldi YL,low(menu_location)	//
 ldi temp,0
 st Y,temp

 LCD_print_str CO_msg		//print first menu page
 second_line
 LCD_print_str CO_msg+96
 .endm
 //------------------------------------------

 //--------Menu previous page----------------
 .macro menu_prev

 push menu_loc			//Save all register which will be used
 push XH				//
 push XL				//
 push temp
 push YH
 push YL

 ldi YH,high(menu_location)		//load to Y register adress of
 ldi YL,low(menu_location)		//current menu page
 ld menu_loc,Y					//read from SRAM current menu page

 ldi temp,0					//if current page is first
 cpse menu_loc,temp			//then going out of procedure
 rjmp PC+2					//else - continue
 rjmp Return_On_2_Click		//

 dec menu_loc				//decrease page because menu moving up
 
 ldi temp,16				//
 mul temp,menu_loc			//counting adress of string
 mov XL,r0					//which contain first string
 mov XH,r1					//of menu message
 ldi temp,0x60				//
 add XL,temp				//adress = ((page - 1)*16)+96
 clr temp					//
 adc XH,temp				//

 first_line					//set first line for print
 LCD_print_str_reg			//print message from adress (in X register)
 
 ldi temp,16				//
 mul temp,menu_loc			//counting adress of string
 mov XL,r0					//which contain second string
 mov XH,r1					//of menu message
 ldi temp,0xC0				//
 add XL,temp				//adress = ((page - 1) * 16)+192
 clr temp					//
 adc XH,temp				//

 second_line				//set second line for print
 LCD_print_str_reg			//print second string

 ldi YH,high(menu_location)	//
 ldi YL,low(menu_location)	//Save current page status to SRAM
 st Y,menu_loc				//

 Return_On_2_Click:			
 pop YL					//return all of used registers
 pop YH					//to their place
 pop temp
 pop XL
 pop XH
 pop menu_loc

 .endm
 //------------------------------------------

 //--------Menu next page--------------------
 .macro menu_next

 push menu_loc		//save to stack
 push XH			//all of used registers
 push XL
 push temp
 push YH
 push YL

 ldi YH,high(menu_location)	//
 ldi YL,low(menu_location)	//read number of current page from SRAM
 ld menu_loc,Y				//

 ldi temp,5					//if this page is last
 cpse menu_loc,temp			//then go to return of procedure
 rjmp PC+2					//else - continue
 rjmp Return_On_8_Click		//

 inc menu_loc		//increase page because moving down
 
 ldi temp,16			//counting of first string adress
 mul temp,menu_loc		//and move it to X register
 mov XL,r0				//
 mov XH,r1				//adress = ((page + 1) * 16)+96
 ldi temp,0x60
 add XL,temp
 clr temp
 adc XH,temp

 first_line			//
 LCD_print_str_reg	//printing first line

 ldi temp,16		//counting of second string adress
 mul temp,menu_loc	//and move it to X register
 mov XL,r0			//
 mov XH,r1			//adress = ((page + 1) * 16)+192
 ldi temp,0xC0
 add XL,temp
 clr temp
 adc XH,temp

 second_line		//
 LCD_print_str_reg	//printing second line

 ldi YH,high(menu_location)	//
 ldi YL,low(menu_location)	//save new current page to SRAM
 st Y,menu_loc				//

 Return_On_8_Click:
 pop YL				//return all of used registers
 pop YH				//
 pop temp
 pop XL
 pop XH
 pop menu_loc

 .endm
 //------------------------------------------

 //      Read from EEPROM and write it in SRAM
 .macro load_strings_to_SRAM
 clr YH		 //load to YH register 0
 ldi YL,0x60 //load to YL 0x60 coz 0x0060 - sram start
 clr ZL		 //clear Z register
 clr ZH
 //?????????????????-----DEBUG------????????????????????????????????????????????????????????????????????
 ldi r19,193			//r19 is a counter
						//USE <LDI r19,97> AFTER DEBUG
 //?????????????????????????????????????????????????????????????????????????????????????????????????????

 EE_read_loop:			//loop 96 times
 rcall EEPROM_read_byte //read byte from EEPROM
 st Y+,byte				//Store byte to SRAM and post-Inc adress
 inc ZL					//increment Z register after reading byte

 dec r19				//loop things...
 brne EE_read_loop		//go to EE_read_loop label if r19 is not 0
 .endm

//--------------------------------------------

 //=========PROCEDURES=====================================
 //------Read Byte form EEPROM-----------------------

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
//==========================================================