/*
 * RTC.inc
 *
 *  Created: 10.06.2016 0:54:10
 *   Author: Hogs
 */ 

 .equ RTCid = 0b11010000	//Clock device id
 .equ CH = 7				//Clock Halt bit
 .equ Hrs = 6				//Hours 12/24 bit

 //========= MACRO ===============================

 //------Correct settings---------------
 //   That macro check clock's setting
 //   and if some settings was wrong
 //   then set it correct

 .macro I2C_Check_Clock_Settings
 push temp
 push r18
 push YH
 push YL

 i2c_read_sbyte RTCid, 0
 
 sbrs r18,7				//skip next command if clock is Off
 rjmp RTC_check_hours

 andi r18, ~(1<<CH)		//set clock halt as 0(enable clock)
 i2c_write_sbyte RTCid, 0, r18
 
 RTC_check_hours:
 i2c_read_sbyte RTCid, 2
 
 sbrs temp,6			//skip next command if 24-hour mode is not set
 rjmp RTC_check_return

 andi r18, ~(1<<Hrs)	//set Hours as 0(24-hours)
 i2c_write_sbyte RTCid, 0, r18

 RTC_check_return:
 pop YL
 pop YH
 pop r18
 pop temp
 .endm

 //----------------------------------

 //------Refresh time-----------
 .macro Refresh_time
 call I2C_Clock_Get_Time
 call Time_to_string
 .endm
 //-----------------------------

 //===============================================

 //========= PROCEDURES ==========================

 //------- Get new time -------------
 I2C_Clock_Get_Time:
 push r18

 //- - - - READ HOURS - - - - - 
 i2c_read_sbyte RTCid, 2		//read hours value
 sts I2C_buffer,r18				//store to I2C_buffer[0]
 
 //- - - - READ MINUTES - - - -
 i2c_read_sbyte RTCid, 1		//read minutes value
 sts I2C_buffer+1,r18			//store to I2C_buffer[1]

 //- - - - READ DATE - - - - -
 i2c_read_sbyte RTCid, 4		//read date value
 sts I2C_buffer+2,r18			//store to I2C_buffer[2]

 //- - - - READ MONTH- - - - - -
 i2c_read_sbyte RTCid, 5		//read month value
 sts I2C_buffer+3,r18			//store to I2C_buffer[3]

 //- - - - READ YEARS - - - - -
 i2c_read_sbyte RTCid, 6		//read year value
 sts I2C_buffer+4,r18			//store to I2C_buffer[4]

 //- - - - - - - - - - - - - -
 pop r18
 ret
 //-----------------------------------------

 //--------- Convert BCD time to string --------------
 Time_to_string:
 push r18
 push r19
 push temp
 push YH
 push YL
 push XH
 push XL

 ldi YH,high(time_set_status)
 ldi YL,low(time_set_status)
 ld temp,Y
 sbrc temp,time_set_S		//if time set mode on then return
 rjmp return_time_to_string

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

 return_time_to_string:
 pop XL
 pop XH
 pop YL
 pop YH
 pop temp
 pop r19
 pop r18
 ret
 //--------------------------------------------

 //-------load I2C_buffer from time value-----------------------
 String_to_time:
 push r18
 push r19
 push temp
 push YH
 push YL
 push XH
 push XL

 clr r18

 ldi YH,high(time_value)
 ldi YL,low(time_value)

 ldi XH,high(I2C_buffer)
 ldi XL,low(I2C_buffer)

 String_to_time_loop:

 ld temp,Y+
 subi temp,48
 swap temp

 ld r19,Y+
 subi r19,48
 or temp,r19

 st X+,temp

 adiw YH:YL,1

 inc r18
 cpi r18,5
 brne String_to_time_loop

 pop XL
 pop XH
 pop YL
 pop YH
 pop temp
 pop r19
 pop r18
 ret
 //----------------------------------------------

 I2C_Clock_Set_New_Time:
 push r18

 //- - - - WRITE HOURS- - - - - -
 lds temp, I2C_buffer
 i2c_write_sbyte RTCid, 2, temp				//write hours

 //- - - - WRITE MINUTES - - - - -
 lds temp, I2C_buffer+1
 i2c_write_sbyte RTCid, 1, temp				//write minutes

 //- - - - WRITE DATE - - - - - -
 lds temp, I2C_buffer+2
 i2c_write_sbyte RTCid, 4, temp				//write date

 //- - - - WRITE MONTH- - - - - - 
 lds temp, I2C_buffer+3
 i2c_write_sbyte RTCid, 5, temp				//write month

 //- - - - WRITE YEARS - - - - - 
 lds temp, I2C_buffer+4
 i2c_write_sbyte RTCid, 6, temp				//write date

 //- - - - - - - - - - - - - - - 
 pop r18
 ret
 //-----------------------------------------

 //===============================================
