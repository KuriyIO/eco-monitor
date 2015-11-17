/*
 * I2C_Real_Time_Clock.inc
 *
 *  Created: 14.11.2015 0:41:13
 *   Author: Hogs
 */ 
 .equ I2C_BS = 7		//bit 7 in I2C status is busy flag (1-busy, 0-free)
 .equ I2C_OP = 6		//bit 6 in I2C status is operation mode(1-read, 0-write)
 .equ I2C_E = 5			//bit 5 in I2C status is error(1 - error occurred, 0 - all is ok)
 .equ I2C_dev_id = 0b11010000	//adress of Real Time Clock

 //========INTERRUPTS=========================================

 //------On I2C interrupt------------------------------
 I2C_Interrupt:
 push temp
 push YH
 push YL

 ldi YH, high(I2C_status)
 ldi YL, low(I2C_status)
 ld temp, Y

 sbrs temp, 6			//skip if operation bit is 1 (read data)
 rjmp I2C_write_data	//if operation bit is 0 (write data) go to I2C_write_data
 
 //- - - - - I2C read - - - - - -

 andi temp, ~((1<<I2C_BS)|(1<<I2C_OP)|(1<<I2C_E))
									//set BS flag, OP flag, E flag as 0, for comfortable 
									//compare of current stage operation process

 cpi temp, 1
 breq stage_1		//go to stage 1(start)
 cpi temp, 2
 breq stage_2		//go to stage 2(device id + W)
 cpi temp, 3
 breq stage_3		//go to stage 3(pointer to clock register)

 cpi temp, 4			//some hack
 in temp,SREG			//the same as:
 sbrc temp, 1			// cpi temp,4
						// breq stage_4_read
						//but have longer branch range
 rjmp stage_4_read	//go to stage 4 for reading data(restart)


 ldi YH, high(I2C_status)
 ldi YL, low(I2C_status)
 ld temp, Y
 andi temp, ~((1<<I2C_BS)|(1<<I2C_OP)|(1<<I2C_E))

 cpi temp, 5			//some hack
 in temp,SREG			//the same as:
 sbrc temp, 1			// cpi temp,6
						// breq stage_5_read
						//but have longer branch range
 rjmp stage_5_read	//go to stage 5 (device id + R)


 ldi YH, high(I2C_status)
 ldi YL, low(I2C_status)
 ld temp, Y
 andi temp, ~((1<<I2C_BS)|(1<<I2C_OP)|(1<<I2C_E))
 cpi temp, 6			//some hack
 in temp,SREG			//the same as:
 sbrc temp, 1			// cpi temp,6
						// breq stage_6_read
						//but have longer branch range
 rjmp stage_6_read	//go to stage 6 for reading data(read data)


 rjmp stop_stage	//else - go to stop stage(generate stop)
 // - - - - - - - - - - - - - - -

 //-------stage 1-----------
 stage_1:
 ldi YH,high(I2C_status)
 ldi YL,low(I2C_status)
 ld temp,Y
 inc temp				//set next stage
 st Y,temp		

 ldi temp, ((1<<TWSTA)|(1<<TWINT)|(1<<TWEN)|(1<<TWIE))	//generating start
 out TWCR,temp

 pop YL
 pop YH
 pop temp
 reti

 //-------stage 2-----------
 stage_2:
 in temp, TWSR
 andi temp, ~((1<<TWPS1) | (1<<TWPS0))	//clear prescaller bits
										//for comfortable compare

 cpi temp,0x08			//some hack, coz brne cannot reach label
 in temp,SREG			//the same as:
 sbrs temp, 1			// cpi temp,0x08
						// brne I2C_error
						//but have longer branch range

 rjmp I2C_error			//if got error than set error code and restart

 ldi YH,high(I2C_status)
 ldi YL,low(I2C_status)
 ld temp,Y
 inc temp				//set next stage
 st Y,temp

 ldi YH,high(I2C_device)
 ldi YL,low(I2C_device)
 ld temp,Y
 andi temp, ~(1<<0)		//set R/W bit as 0 for write
 out TWDR,temp			//load to TWDR SLA+W(device id + write)

 ldi temp, ((1<<TWINT)|(1<<TWEN)|(1<<TWIE))	//send SLA+W(device id + write)
 out TWCR,temp

 pop YL
 pop YH
 pop temp
 reti

 //-------stage 3-----------
 stage_3:
 in temp, TWSR
 andi temp, ~((1<<TWPS1) | (1<<TWPS0))	//clear prescaller bits
										//for comfortable compare
 cpi temp,0x18
 in temp,SREG			//the same as:
 sbrs temp, 1			// cpi temp,0x18
						// brne I2C_error
						//but have longer branch range

 rjmp I2C_error			//if got error than set error code and restart

 ldi YH,high(I2C_status)
 ldi YL,low(I2C_status)
 ld temp,Y
 inc temp				//set next stage
 st Y,temp

 ldi YH,high(I2C_data_pointer)
 ldi YL,low(I2C_data_pointer)
 ld temp,Y
 out TWDR,temp			//load to data pointer

 ldi temp, ((1<<TWINT)|(1<<TWEN)|(1<<TWIE))	//send data pointer
 out TWCR,temp

 pop YL
 pop YH
 pop temp
 reti

 //-------stage 4 read------
 stage_4_read:
 in temp, TWSR
 andi temp, ~((1<<TWPS1) | (1<<TWPS0))	//clear prescaller bits
										//for comfortable compare
 cpi temp,0x28
 brne I2C_error			//if got error than set error code and restart

 ldi YH,high(I2C_status)
 ldi YL,low(I2C_status)
 ld temp,Y
 inc temp				//set next stage
 st Y,temp

 ldi temp, ((1<<TWSTA)|(1<<TWINT)|(1<<TWEN)|(1<<TWIE))	//generating start
 out TWCR,temp

 pop YL
 pop YH
 pop temp
 reti

 //-------stage 5 read------
 stage_5_read:
 in temp, TWSR
 andi temp, ~((1<<TWPS1) | (1<<TWPS0))	//clear prescaller bits
										//for comfortable compare
 cpi temp,0x10
 brne I2C_error			//if got error than set error code and restart

 ldi YH,high(I2C_status)
 ldi YL,low(I2C_status)
 ld temp,Y
 inc temp				//set next stage
 st Y,temp

 ldi YH,high(I2C_device)
 ldi YL,low(I2C_device)
 ld temp,Y
 ori temp, (1<<0)		//set R/W bit as 1 for read
 out TWDR,temp			//load to TWDR SLA+R(device id + read)

 ldi temp, ((1<<TWINT)|(1<<TWEN)|(1<<TWIE))	//send SLA+R(device id + read)
 out TWCR,temp

 pop YL
 pop YH
 pop temp
 reti

 //-------stage 6 read------
 stage_6_read:
 in temp, TWSR
 andi temp, ~((1<<TWPS1) | (1<<TWPS0))	//clear prescaller bits
										//for comfortable compare
 cpi temp,0x40
 brne I2C_error			//if got error than set error code and restart

 ldi YH,high(I2C_status)
 ldi YL,low(I2C_status)
 ld temp,Y
 inc temp				//set next stage
 st Y,temp

 ldi temp, ((1<<TWINT)|(1<<TWEN)|(1<<TWIE))	//send data pointer
 out TWCR,temp

 pop YL
 pop YH
 pop temp
 reti

 //-------I2C error---------
 I2C_error:
 ldi YH,high(I2C_status)
 ldi YL,low(I2C_status)
 ld temp,Y
 andi temp, ~(1<<I2C_BS)//clear busy flag
 ori temp,(1<<I2C_E)	//set error flag
 st Y,temp				//save error to I2C status
						//for restart operation
						//and after that send STOP

 ldi temp, ((1<<TWSTO)|(1<<TWINT)|(1<<TWEN)|(1<<TWIE))	//generating stop
 out TWCR,temp

 pop YL
 pop YH
 pop temp
 reti
 //-------stop stage--------
 stop_stage:
 ldi YH,high(I2C_status)
 ldi YL,low(I2C_status)
 ld temp,Y
 clr temp		//set current stage 0(operation end) also clear all flags
 st Y,temp

 /////////////////////////////////////////////////////////////////////////////////////////////////
 ldi YH,high(I2C_data_read)
 ldi YL,low(I2C_data_read)

 in temp,TWDR			//load to temp just readed data
 st Y,temp				//save it to I2C_data_read
 clr temp
 out TWDR,temp			//clear TWDR
 /////////////////////////////////////////////////////////////////////////////////////////////////
 ldi temp, ((1<<TWSTO)|(1<<TWINT)|(1<<TWEN)|(1<<TWIE))	//generating stop
 out TWCR,temp

 pop YL
 pop YH
 pop temp
 reti

 //-------stage 4 write------
 stage_4_write:
 in temp, TWSR
 andi temp, ~((1<<TWPS1) | (1<<TWPS0))	//clear prescaller bits
										//for comfortable compare
 cpi temp,0x28
 brne I2C_error			//if got error than set error code and restart

 ldi YH,high(I2C_status)
 ldi YL,low(I2C_status)
 ld temp,Y
 inc temp				//set next stage
 st Y,temp

 ldi YH,high(I2C_data_write)
 ldi YL,low(I2C_data_write)
 ld temp,Y
 out TWDR,temp			//load data

 ldi temp, ((1<<TWINT)|(1<<TWEN)|(1<<TWIE))	//send data
 out TWCR,temp

 pop YL
 pop YH
 pop temp
 reti

 //-------------------------

 //- - - - - I2C write - - - - - -
 I2C_write_data:

 andi temp, ~((1<<I2C_BS)|(1<<I2C_OP)|(1<<I2C_E))
									//set BS flag, OP flag, E flag as 0, for comfortable 
									//compare of current stage operation process

 cpi temp, 1
 in temp,SREG			//the same as:
 sbrc temp, 1			// cpi temp,1
						// breq stage_1
						//but have longer branch range

 rjmp stage_1		//go to stage 1(start)


 ldi YH, high(I2C_status)
 ldi YL, low(I2C_status)
 ld temp, Y
 andi temp, ~((1<<I2C_BS)|(1<<I2C_OP)|(1<<I2C_E))
 cpi temp, 2
 in temp,SREG			//the same as:
 sbrc temp, 1			// cpi temp,2
						// breq stage_2
						//but have longer branch range
 rjmp stage_2		//go to stage 2(device id + W)


 ldi YH, high(I2C_status)
 ldi YL, low(I2C_status)
 ld temp, Y
 andi temp, ~((1<<I2C_BS)|(1<<I2C_OP)|(1<<I2C_E))
 cpi temp, 3
 in temp,SREG			//the same as:
 sbrc temp, 1			// cpi temp,3
						// breq stage_3
						//but have longer branch range
 rjmp stage_3		//go to stage 3(pointer to clock's register)


 ldi YH, high(I2C_status)
 ldi YL, low(I2C_status)
 ld temp, Y
 andi temp, ~((1<<I2C_BS)|(1<<I2C_OP)|(1<<I2C_E))
 cpi temp, 4
 in temp,SREG			//the same as:
 sbrc temp, 1			// cpi temp,4
						// breq stage_4_write
						//but have longer branch range
 rjmp stage_4_write	//go to stage 4 for write data(write data)

 rjmp stop_stage	//else - go to stop stage(generate stop)
 // - - - - - - - - - - - - - - -
 
 //----------------------------------------------------

 //=========MACRO============================================
 
 //------I2C Init-----------------------
 .macro I2C_Init
 ldi temp,60
 out TWBR,temp		//set SCL frequency as 100KHz

 ldi YH,high(I2C_device)
 ldi YL,low(I2C_device)
 ldi temp,I2C_dev_id	//save to RAM Clock id
 st Y,temp

 .endm
 //-------------------------------------

 //------Correct settings---------------
 //   That macro check clock's setting
 //   and if some settings was wrong
 //   then set it correct

 .macro I2C_Check_Clock_Settings
 push temp
 push YH
 push YL

 ldi YH,high(I2C_data_pointer)
 ldi YL,low(I2C_data_pointer)
 ldi temp,0x00
 st Y,temp
 call I2C_read_byte

 ldi YH,high(I2C_data_read)
 ldi YL,low(I2C_data_read)
 ld temp,Y
 
 sbrc temp,7	//skip next command if clock is On
 call I2C_Clock_Enable

 ldi YH,high(I2C_data_pointer)
 ldi YL,low(I2C_data_pointer)
 ldi temp,0x02
 st Y,temp
 call I2C_read_byte

 ldi YH,high(I2C_data_read)
 ldi YL,low(I2C_data_read)
 ld temp,Y
 
 sbrc temp,6	//skip next command if 24-hour mode is set
 call I2C_Clock_Set_24Hour

 pop YL
 pop YH
 pop temp
 .endm

 //-------------------------------------

 //-------Set new time----------------------
 I2C_Clock_Set_New_Time:
 push temp
 push r16
 push YH
 push YL

 //- - - - - - - - - - - - WRITE HOURS- - - - - - - - - - - - - - - - - - - - - - - - - - 
 clr r16
 ldi YH,high(I2C_buffer)
 ldi YL,low(I2C_buffer)
 ld temp,Y

 ldi YH,high(I2C_data_write)
 ldi YL,low(I2C_data_write)
 st Y,temp							//save to I2C_data_write value from I2C_buffer+1

 ldi YH,high(I2C_data_pointer)
 ldi YL,low(I2C_data_pointer)
 ldi temp,0x02
 st Y,temp							//write to 0x02 adress

 rcall I2C_write_byte				//write hours

 //- - - - - - - - - - - - -WRITE MINUTES - - - - - - - - - - - - - - - - - - - - - - - - 
 inc r16
 ldi YH,high(I2C_buffer)
 ldi YL,low(I2C_buffer)
 add YL,r16
 push r16
 clr r16
 adc YH,r16
 pop r16
 ld temp,Y

 ldi YH,high(I2C_data_write)
 ldi YL,low(I2C_data_write)
 st Y,temp						//save to I2C_data_write value from I2C_buffer+0

 ldi YH,high(I2C_data_pointer)
 ldi YL,low(I2C_data_pointer)
 ldi temp,0x01
 st Y,temp						//write to 0x01 adress

 rcall I2C_write_byte			//write minutes

 //- - - - - - - - - - - - WRITE DATE - - - - - - - - - - - - - - - - - - - - - - - 
 inc r16
 ldi YH,high(I2C_buffer)
 ldi YL,low(I2C_buffer)
 add YL,r16
 push r16
 clr r16
 adc YH,r16
 pop r16
 ld temp,Y

 ldi YH,high(I2C_data_write)
 ldi YL,low(I2C_data_write)
 st Y,temp							//save to I2C_data_write value from I2C_buffer+2

 ldi YH,high(I2C_data_pointer)
 ldi YL,low(I2C_data_pointer)
 ldi temp,0x04
 st Y,temp							//write to 0x04 adress

 rcall I2C_write_byte				//write date

 //- - - - - - - - - - - WRITE MONTH- - - - - - - - - - - - - - - - - - - - - - - - - - 
 inc r16
 ldi YH,high(I2C_buffer)
 ldi YL,low(I2C_buffer)
 add YL,r16
 push r16
 clr r16
 adc YH,r16
 pop r16
 ld temp,Y

 ldi YH,high(I2C_data_write)
 ldi YL,low(I2C_data_write)
 st Y,temp							//save to I2C_data_write value from I2C_buffer+3

 ldi YH,high(I2C_data_pointer)
 ldi YL,low(I2C_data_pointer)
 ldi temp,0x05
 st Y,temp							//write to 0x05 adress

 rcall I2C_write_byte				//write month

 //- - - - - - - - - - - -WRITE YEARS - - - - - - - - - - - - - - - - - - - - - - - - - 
 inc r16
 ldi YH,high(I2C_buffer)
 ldi YL,low(I2C_buffer)
 add YL,r16
 push r16
 clr r16
 adc YH,r16
 pop r16
 ld temp,Y

 ldi YH,high(I2C_data_write)
 ldi YL,low(I2C_data_write)
 st Y,temp							//save to I2C_data_write value from I2C_buffer+4

 ldi YH,high(I2C_data_pointer)
 ldi YL,low(I2C_data_pointer)
 ldi temp,0x05
 st Y,temp							//write to 0x06 adress

 rcall I2C_write_byte				//write years
 //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
 pop YL
 pop YH
 pop r16
 pop temp
 ret
 //-----------------------------------------

 //-------Get new time----------------------
 I2C_Clock_Get_Time:
 push temp
 push r16
 push YH
 push YL

 clr r16
 //- - - - - - - - - - - - READ HOURS- - - - - - - - - - - - - - - - - - - - - - - - - - 
 ldi YH,high(I2C_data_pointer)
 ldi YL,low(I2C_data_pointer)
 ldi temp,0x02
 st Y,temp							//read from 0x02 adress

 rcall I2C_read_byte				//read hours to I2C_data_read

 ldi YH,high(I2C_data_read)
 ldi YL,low(I2C_data_read)
 ld temp,Y							//load to temp just readed data

 ldi YH,high(I2C_buffer)
 ldi YL,low(I2C_buffer)
 st Y,temp						//store temp to I2C_buffer[0]
 
 //- - - - - - - - - - - - -READ MINUTES - - - - - - - - - - - - - - - - - - - - - - - - 
 ldi YH,high(I2C_data_pointer)
 ldi YL,low(I2C_data_pointer)
 ldi temp,0x01
 st Y,temp						//read from 0x01 adress

 rcall I2C_read_byte			//read minutes to I2C_data_read

 ldi YH,high(I2C_data_read)
 ldi YL,low(I2C_data_read)
 ld temp,Y						//load I2C_data_read to temp

 inc r16
 ldi YH,high(I2C_buffer)
 ldi YL,low(I2C_buffer)
 add YL,r16
 push r16
 clr r16
 adc YH,r16
 pop r16
 st Y,temp							//store temp to buffer[1]

 //- - - - - - - - - - - - READ DATE - - - - - - - - - - - - - - - - - - - - - - - 
 ldi YH,high(I2C_data_pointer)
 ldi YL,low(I2C_data_pointer)
 ldi temp,0x04
 st Y,temp							//read from 0x04 adress

 rcall I2C_read_byte				//read date to I2C_data_read

 ldi YH,high(I2C_data_read)
 ldi YL,low(I2C_data_read)
 ld temp,Y							//load to temp just readed data

 inc r16
 ldi YH,high(I2C_buffer)
 ldi YL,low(I2C_buffer)
 add YL,r16
 push r16
 clr r16
 adc YH,r16
 pop r16
 st Y,temp							//store temp to buffer[2]

 //- - - - - - - - - - - READ MONTH- - - - - - - - - - - - - - - - - - - - - - - - - - 
 ldi YH,high(I2C_data_pointer)
 ldi YL,low(I2C_data_pointer)
 ldi temp,0x05
 st Y,temp							//read from 0x05 adress

 rcall I2C_read_byte				//read month to I2C_data_read

 ldi YH,high(I2C_data_read)
 ldi YL,low(I2C_data_read)
 ld temp,Y							//load to temp just readed data

 inc r16
 ldi YH,high(I2C_buffer)
 ldi YL,low(I2C_buffer)
 add YL,r16
 push r16
 clr r16
 adc YH,r16
 pop r16
 st Y,temp							//store temp to buffer[3]

 //- - - - - - - - - - - -READ YEARS - - - - - - - - - - - - - - - - - - - - - - - - - 
 ldi YH,high(I2C_data_pointer)
 ldi YL,low(I2C_data_pointer)
 ldi temp,0x06
 st Y,temp							//read from 0x06 adress

 rcall I2C_read_byte				//read years to I2C_data_read

 ldi YH,high(I2C_data_read)
 ldi YL,low(I2C_data_read)
 ld temp,Y							//load to temp just readed data

 inc r16
 ldi YH,high(I2C_buffer)
 ldi YL,low(I2C_buffer)
 add YL,r16
 push r16
 clr r16
 adc YH,r16
 pop r16
 st Y,temp							//store temp to buffer[4]
 //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
 pop YL
 pop YH
 pop r16
 pop temp
 ret
 //-----------------------------------------
 //------Refresh time-------------------------------------------
 .macro Refresh_time
 call I2C_Clock_Get_Time
 call Time_to_string
 .endm
 //-------------------------------------------------------------
 //======PROCEDURES=========================================

 //-------load time value from I2C_buffer-----------------------
 Time_to_string:
 push r18
 push r19
 push temp
 push YH
 push YL
 push XH
 push XL

 ldi r19,48
 clr r18

 ldi YH,high(time_value)
 ldi YL,low(time_value)

 ldi XH,high(I2C_buffer)
 ldi XL,low(I2C_buffer)

 Time_to_string_loop:

 ld temp,X+

 push temp
 swap temp
 andi temp,0x0F	//clear high half-byte
 add temp,r19	//temp += 48
 st Y+,temp

 pop temp
 andi temp,0x0F	//clear high half-byte
 add temp,r19	//temp += 48
 st Y+,temp

 adiw YH:YL,1

 inc r18
 cpi r18,5
 brne Time_to_string_loop

 pop XL
 pop XH
 pop YL
 pop YH
 pop temp
 pop r19
 pop r18
 ret
 //-------------------------------------------------------------
 //-----Write byte------------------
 I2C_write_byte:
 push temp
 push YH
 push YL

 I2C_error_restart_write:

 ldi YH,high(I2C_status)
 ldi YL,low(I2C_status)
 ld temp,Y
 sbrc temp,I2C_BS					//wait for last I2C operation end
 rjmp I2C_error_restart_write

 ldi temp, (1<<I2C_BS)				//set I2C busy and write
 inc temp							//set current stage as 1
 st Y,temp		//save to I2C_data_pointer a pointer to register
				//which need to read
				
 rcall I2C_Interrupt

 I2C_wait_write_end:

 ldi YH,high(I2C_status)
 ldi YL,low(I2C_status)
 ld temp,Y

 sbrc temp, I2C_E				//if error occurred
 rjmp I2C_error_restart_write	//restart read

 andi temp, 0b0001_1111		//clear flags, for comfortable compare
 cpi temp,0
 brne I2C_wait_write_end

 pop YL
 pop YH
 pop temp
 ret
  //-----Read byte-------------------
 I2C_read_byte:
 push temp
 push YH
 push YL

 I2C_error_restart_read:

 ldi YH,high(I2C_status)
 ldi YL,low(I2C_status)
 ld temp,Y
 sbrc temp,I2C_BS					//wait for last I2C operation end
 rjmp I2C_error_restart_read

 ldi temp, (1<<I2C_BS)|(1<<I2C_OP)	//set I2C busy and read
 inc temp							//set current stage as 1
 st Y,temp		//save to I2C_data_pointer a pointer to register
				//which need to read
				
 rcall I2C_Interrupt

 I2C_wait_read_end:

 ldi YH,high(I2C_status)
 ldi YL,low(I2C_status)
 ld temp,Y

 sbrc temp, I2C_E			//if error occurred
 rjmp I2C_error_restart_read	//restart read

 andi temp, 0b0001_1111		//clear flags, for comfortable compare
 cpi temp,0
 brne I2C_wait_read_end

 pop YL
 pop YH
 pop temp
 ret

 //----Set Clock Halt(enable clock)------------------
 I2C_Clock_Enable:
 push temp
 push YH
 push YL

 andi temp, ~(1<<7)	//set clock halt as 0(enable clock)
 ldi YH,high(I2C_data_write)
 ldi YL,low(I2C_data_write)
 st Y,temp

 ldi YH,high(I2C_data_pointer)
 ldi YL,low(I2C_data_pointer)
 ldi temp,0x00
 st Y,temp

 rcall I2C_write_byte

 pop YL
 pop YH
 pop temp
 ret
 //--------------------------------------------------

 //----Set 24-hour mode------------------------------
 I2C_Clock_Set_24Hour:
 push temp
 push YH
 push YL

 andi temp, ~(1<<6)	//set hour mode as 0(24-hour mode)
 ldi YH,high(I2C_data_write)
 ldi YL,low(I2C_data_write)
 st Y,temp

 ldi YH,high(I2C_data_pointer)
 ldi YL,low(I2C_data_pointer)
 ldi temp,0x02
 st Y,temp

 rcall I2C_write_byte

 pop YL
 pop YH
 pop temp
 ret
 //--------------------------------------------------
 //=========================================================