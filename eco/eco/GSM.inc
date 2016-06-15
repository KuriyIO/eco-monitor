/*
 * GSM.inc
 *
 *  Created: 30.03.2016 0:25:56
 *   Author: Hogs
 */ 

 .equ GS = 3		//get/set flag in GSM_Status register (1 - get, 0 - set)

 //======= MACRO ================================
 //------- Copy index to RAM --------------------
 .macro GSM_copy_index

 ldi XH, high(UART_RX_buffer)
 ldi XL, low(UART_RX_buffer)
 add XL, byte
 push byte
 clr byte
 adc XH, byte
 pop byte
 adiw XH:XL, 12
 ldi YH, high(GSM_sms_index)
 ldi YL, low(GSM_sms_index)
 ldi byte, 0x0D
 rcall str_cpy_proc

 .endm
 //----------------------------------------------

 //------- Copy phone number to RAM -------------
 .macro GSM_copy_phone_number

 ldi XH, high(UART_RX_buffer)
 ldi XL, low(UART_RX_buffer)
 add XL, byte
 push byte
 clr byte
 adc XH, byte
 pop byte
 adiw XH:XL, 2
 ldi YH, high(GSM_phone_number)
 ldi YL, low(GSM_phone_number)
 ldi byte, ','
 rcall str_cpy_proc

 .endm
 //----------------------------------------------

 //------- Set New Time -------------------------
 .macro GSM_set_new_time

 push XH
 push XL
 push YH
 push YL
 push temp
 push byte

 push XH
 push XL
 push YH
 push YL
 push byte

 mov XH, YH
 mov XL, YL
 ldi YH, high(time_value)
 ldi YL, low(time_value)
 ldi byte, 14
 rcall str_cpy_proc_len

 pop byte
 pop YL
 pop YH
 pop XL
 pop XH

 ldi XH, high(I2C_buffer)
 ldi XL, low(I2C_buffer)
 
 ldi temp, 5
 GSM_set_new_time_loop:
 push temp

 ld temp, Y+
 subi temp, 48
 swap temp

 ld byte, Y+
 subi byte, 48

 or temp, byte
 st X+, temp

 adiw YH:YL, 1

 pop temp
 dec temp
 brne GSM_set_new_time_loop

 call I2C_Clock_Set_New_Time

 pop byte
 pop temp
 pop YL
 pop YH
 pop XL
 pop XH
 .endm
 //--------------------------------------------

 //--------- GSM init -------------------------
 .macro GSM_init

 call GSM_init_strings

 UART_clear_TX_buffer	//Clear TX buffer
 UART_start_receive

 str_concat UART_TX_buffer, GSM_CSQ
 UART_start_transmit
 
 delay 255,255,255
 //??????????????????????????????????????????????????//
 delay 255,255,255
 delay 255,255,255
 str_search UART_RX_buffer, GSM_result
 cpi temp, 1
 brne dbg_1
 dbg
 dbg_1:

 //?????????????????????????????????????????????????????????
 UART_clear_RX_buffer
 UART_clear_TX_buffer

 UART_start_receive
 .endm
 //--------------------------------------------

 //------- main -------------------------------
 //.macro GSM_main
 GSM_main:
 UART_start_receive

 /*UART_clear_TX_buffer
 str_concat UART_TX_buffer, GSM_CSQ
 UART_start_transmit
 
 delay 255,255,255
 delay 255,255,255
 delay 255,255,255
 UART_clear_TX_buffer

 str_search UART_RX_buffer, GSM_result
 cpi temp, 1
 brne dbg_1
 dbg
 dbg_1:*/
 /* check for new SMS */ 

 
 str_search UART_RX_buffer, GSM_new_msg
 cpi temp, 0
 brne PC+2 
 jmp GSM_return	//if no new SMS found return

 dbg

 GSM_copy_index		//copy index to RAM

 /* read message */
 GSM_restart:
 UART_clear_RX_buffer
 UART_clear_TX_buffer	//Clear TX buffer

 str_concat UART_TX_buffer, GSM_read_SMS1
 str_concat UART_TX_buffer, GSM_sms_index	//prepare TX buffer
 str_concat UART_TX_buffer, GSM_read_SMS2
 str_concat UART_TX_buffer, GSM_enter

 //dbg

 UART_start_transmit 
 UART_start_receive

 //dbg

 /* wait for operation result */
 delay 255,255,255		//long delay
 delay 255,255,255
 delay 255,255,255
 delay 255,255,255
 delay 255,255,255
 delay 255,255,255		//about 6 sec on 16MHz or 12sec on 8MHz
 //UART_stop_receive
 str_search UART_RX_buffer, GSM_result
 cpi temp, 0
 brne PC+2
 rjmp GSM_restart		//if no OK result then restart

 
 str_search UART_RX_buffer, GSM_phone_start
 cpi temp, 0
 brne PC+2
 rjmp GSM_restart	//if no new SMS found restart
 
 GSM_copy_phone_number

 /* check for commands */
 str_search UART_RX_buffer, GSM_get_cmd
 cpi temp, 0
 brne GSM_get_parameter

 str_search UART_RX_buffer, GSM_set_cmd
 cpi temp, 0
 breq PC+2
 rjmp GSM_set_parameter
 jmp GSM_return

 GSM_get_parameter:
 ldi YH, high(GSM_Status)
 ldi YL, low(GSM_Status)
 ldi temp, 1<<GS									//set GS flag as 1 (get)
 st Y, temp
 
 str_search UART_RX_buffer, GSM_info_parameter		//search for "info" parameter
 cpi temp, 0
 breq GSM_search_time_get_parameter

 ldi YH, high(GSM_Status)
 ldi YL, low(GSM_Status)
 ld temp, Y
 ori temp, 1
 st Y, temp
 rjmp GSM_send_SMS_command

 GSM_search_time_get_parameter:
 str_search UART_RX_buffer, GSM_time_parameter		//search for "time" parameter
 cpi temp, 0
 breq GSM_search_temp_get_parameter

 ldi YH, high(GSM_Status)
 ldi YL, low(GSM_Status)
 ld temp, Y
 ori temp, 2
 st Y, temp
 rjmp GSM_send_SMS_command

 GSM_search_temp_get_parameter:
 str_search UART_RX_buffer, GSM_temp_parameter		//search for "temp" parameter
 cpi temp, 0
 breq GSM_search_pressure_get_parameter

 ldi YH, high(GSM_Status)
 ldi YL, low(GSM_Status)
 ld temp, Y
 ori temp, 3
 st Y, temp
 rjmp GSM_send_SMS_command

 GSM_search_pressure_get_parameter:
 str_search UART_RX_buffer, GSM_pressure_parameter	//search for "pressure" parameter
 cpi temp, 0
 breq GSM_search_sound_get_parameter

 ldi YH, high(GSM_Status)
 ldi YL, low(GSM_Status)
 ld temp, Y
 ori temp, 4
 st Y, temp
 rjmp GSM_send_SMS_command

 GSM_search_sound_get_parameter:
 str_search UART_RX_buffer, GSM_sound_parameter		//search for "sound" parameter
 cpi temp, 0
 breq GSM_search_memory_get_parameter

 ldi YH, high(GSM_Status)
 ldi YL, low(GSM_Status)
 ld temp, Y
 ori temp, 5
 st Y, temp
 rjmp GSM_send_SMS_command

 GSM_search_memory_get_parameter:
 str_search UART_RX_buffer, GSM_memory_parameter	//search for "memory" parameter
 cpi temp, 0
 breq GSM_search_unsupported_get_parameter

 ldi YH, high(GSM_Status)
 ldi YL, low(GSM_Status)
 ld temp, Y
 ori temp, 6
 st Y, temp

 GSM_search_unsupported_get_parameter:				//unsupported parameter
 rjmp GSM_send_SMS_command


 GSM_set_parameter:
 ldi YH, high(GSM_Status)
 ldi YL, low(GSM_Status)
 clr temp
 st Y, temp

 str_search UART_RX_buffer, GSM_time_parameter		//search for "time" parameter
 cpi temp, 0
 brne PC+2
 rjmp GSM_search_sound_set_parameter

 push temp
 ldi temp, 5
 add byte, temp
 pop temp
 ldi YH, high(UART_RX_buffer)
 ldi YL, low(UART_RX_buffer)
 add YL, byte
 clr byte
 adc YH, byte

 call GSM_time_validate
 cpi temp, 0
 breq GSM_set_time_error

 GSM_set_new_time

 ldi YH, high(GSM_Status)
 ldi YL, low(GSM_Status)
 ld temp, Y
 ori temp, 2
 st Y, temp
 rjmp GSM_send_SMS_command

 GSM_set_time_error:
 ldi YH, high(GSM_Status)
 ldi YL, low(GSM_Status)
 ld temp, Y
 ori temp, 0b0001_0000
 st Y, temp
 rjmp GSM_send_SMS_command

 GSM_search_sound_set_parameter:
 str_search UART_RX_buffer, GSM_sound_parameter		//search for "sound" parameter
 cpi temp, 0
 brne PC+2
 rjmp GSM_search_memory_set_parameter

 push temp
 ldi temp, 6
 add byte, temp
 pop temp
 ldi YH, high(UART_RX_buffer)
 ldi YL, low(UART_RX_buffer)
 add YL, byte
 clr byte
 adc YH, byte

 call GSM_OnOff_Validate
 cpi temp, 0
 breq GSM_set_sound_error

 adiw YH:YL, 1
 ld temp, Y
 cpi temp, 'F'
 breq GSM_set_sound_off

 ldi YH,high(Sound_Settings)
 ldi YL,low(Sound_Settings) 
 ldi temp, 1
 st Y, temp
 str_cpy On_str, sound_value
 rjmp GSM_set_sound_return

 GSM_set_sound_off:
 ldi YH,high(Sound_Settings)
 ldi YL,low(Sound_Settings) 
 ldi temp, 0
 st Y, temp
 str_cpy Off_str, sound_value

 GSM_set_sound_return:
 ldi YH, high(GSM_Status)
 ldi YL, low(GSM_Status)
 ld temp, Y
 ori temp, 5
 st Y, temp
 rjmp GSM_send_SMS_command

 GSM_set_sound_error:
 ldi YH, high(GSM_Status)
 ldi YL, low(GSM_Status)
 ld temp, Y
 ori temp, 0b0010_0000
 st Y, temp
 rjmp GSM_send_SMS_command

 GSM_search_memory_set_parameter:
 str_search UART_RX_buffer, GSM_memory_parameter	//search for "memory" parameter
 cpi temp, 0
 brne PC+2
 rjmp GSM_search_unsupported_set_parameter

 push temp
 ldi temp, 7
 add byte, temp
 pop temp
 ldi YH, high(UART_RX_buffer)
 ldi YL, low(UART_RX_buffer)
 add YL, byte
 clr byte
 adc YH, byte

 call GSM_OnOff_Validate
 cpi temp, 0
 breq GSM_set_memory_error

 adiw YH:YL, 1
 ld temp, Y
 cpi temp, 'F'
 breq GSM_set_memory_off

 ldi YH, high(MC_Settings)
 ldi YL, low(MC_Settings)
 ldi temp, 1
 st Y, temp
 str_cpy On_str, mc_value
 rjmp GSM_set_memory_return

 GSM_set_memory_off:
 ldi YH, high(MC_Settings)
 ldi YL, low(MC_Settings)
 ldi temp, 0
 str_cpy Off_str, mc_value
 st Y, temp

 GSM_set_memory_return:
 ldi YH, high(GSM_Status)
 ldi YL, low(GSM_Status)
 ld temp, Y
 ori temp, 6
 st Y, temp
 rjmp GSM_send_SMS_command

 GSM_set_memory_error:
 ldi YH, high(GSM_Status)
 ldi YL, low(GSM_Status)
 ld temp, Y
 ori temp, 0b0010_0000
 st Y, temp

 GSM_search_unsupported_set_parameter:
 
 /* SMS send command */
 GSM_send_SMS_command:
 UART_clear_TX_buffer	//Clear TX buffer
 UART_clear_RX_buffer	//Clear TX buffer
 //str_cpy GSM_phone_number, pressure_msg
 //menu_refresh

 str_concat UART_TX_buffer, GSM_write
 str_concat UART_TX_buffer, GSM_phone_number	//prepare TX buffer
 str_concat UART_TX_buffer, GSM_enter

 UART_start_transmit
 UART_start_receive

 delay 255,255,255		//long delay
 //UART_stop_receive

 str_search UART_RX_buffer, GSM_write_msg
 cpi temp, 0
 brne PC+2
 rjmp GSM_send_SMS_command

 sbi PORTD, 7//?????????????????????????????????????????????????????

 
 /* write answer message */
 UART_clear_TX_buffer	//Clear TX buffer

 ldi XH, high(GSM_Status)
 ldi XL, low(GSM_Status)
 ld temp, X

 sbrs temp, 3	//check G/S bit
 rjmp GSM_answer_set	//go to GSM_answer_set if G/S bit is 0

 //if G/S is 1 (get)
 andi temp, 0b0000_0111
 cpi temp, 1
 breq GSM_answer_AllInfo
 cpi temp, 2
 brne PC+2
 rjmp GSM_answer_time
 cpi temp, 3
 brne PC+2
 rjmp GSM_answer_temp
 cpi temp, 4
 brne PC+2
 rjmp GSM_answer_pressure
 cpi temp, 5
 brne PC+2
 rjmp GSM_answer_sound
 cpi temp, 6
 brne PC+2
 rjmp GSM_answer_memory
 rjmp GSM_answer_invalid_get

 GSM_answer_set:
 //if G/S is 0 (set)

 sbrc temp, 4	//check for CE(code error) flags
 rjmp GSM_answer_invalid_time	
 sbrc temp, 5
 rjmp GSM_answer_invalid_OnOff

 andi temp, 0b0000_0111
 cpi temp, 0	//check for correct set
 brne PC+2
 rjmp GSM_answer_invalid_set
 rjmp GSM_answer_OK	//else: go to OK

 GSM_answer_AllInfo:
 UART_clear_TX_buffer	//Clear TX buffer
 str_concat_ex UART_TX_buffer, pressure_msg, 2
 str_concat_ex UART_TX_buffer, pressure_value, 1
 str_concat UART_TX_buffer, GSM_enter
 str_concat UART_TX_buffer, temperature_msg
 str_concat_ex UART_TX_buffer, temperature_value, 1
 str_concat UART_TX_buffer, GSM_enter
 str_concat UART_TX_buffer, time_msg
 str_concat_ex UART_TX_buffer, time_value, 1
 str_concat UART_TX_buffer, GSM_enter
 str_concat UART_TX_buffer, mc_msg
 str_concat_ex UART_TX_buffer, mc_value, 1
 str_concat UART_TX_buffer, GSM_enter
 str_concat UART_TX_buffer, sound_msg
 str_concat_ex UART_TX_buffer, sound_value, 2
 rjmp GSM_answer_continue

 GSM_answer_time:
 UART_clear_TX_buffer	//Clear TX buffer
 str_concat UART_TX_buffer, time_msg
 str_concat UART_TX_buffer, GSM_enter
 str_concat_ex UART_TX_buffer, time_value, 1
 rjmp GSM_answer_continue
 
 GSM_answer_temp:
 UART_clear_TX_buffer	//Clear TX buffer
 str_concat UART_TX_buffer, temperature_msg
 str_concat UART_TX_buffer, GSM_enter
 str_concat_ex UART_TX_buffer, temperature_value, 1
 rjmp GSM_answer_continue

 GSM_answer_pressure:
 UART_clear_TX_buffer	//Clear TX buffer
 str_concat_ex UART_TX_buffer, pressure_msg, 2
 str_concat UART_TX_buffer, GSM_enter
 str_concat_ex UART_TX_buffer, pressure_value, 1
 rjmp GSM_answer_continue

 GSM_answer_sound:
 UART_clear_TX_buffer	//Clear TX buffer
 str_concat UART_TX_buffer, sound_msg
 str_concat UART_TX_buffer, GSM_enter
 str_concat_ex UART_TX_buffer, sound_value, 2
 rjmp GSM_answer_continue

 GSM_answer_memory:
 UART_clear_TX_buffer	//Clear TX buffer
 str_concat UART_TX_buffer, mc_msg
 str_concat UART_TX_buffer, GSM_enter
 str_concat_ex UART_TX_buffer, mc_value, 1
 rjmp GSM_answer_continue

 GSM_answer_invalid_get:
 UART_clear_TX_buffer	//Clear TX buffer
 str_concat UART_TX_buffer, GSM_invalid_get
 rjmp GSM_answer_continue

 GSM_answer_invalid_time:
 UART_clear_TX_buffer	//Clear TX buffer
 str_concat UART_TX_buffer, GSM_invalid_time_msg
 rjmp GSM_answer_continue

 GSM_answer_invalid_OnOff:
 UART_clear_TX_buffer	//Clear TX buffer
 str_concat UART_TX_buffer, GSM_invalid_OnOff
 rjmp GSM_answer_continue

 GSM_answer_invalid_set:
 UART_clear_TX_buffer	//Clear TX buffer
 str_concat UART_TX_buffer, GSM_invalid_set
 rjmp GSM_answer_continue

 GSM_answer_OK:
 UART_clear_TX_buffer	//Clear TX buffer
 str_concat UART_TX_buffer, GSM_result
 rjmp GSM_answer_continue

 GSM_answer_continue:

 //UART_clear_TX_buffer	//Clear TX buffer
 //str_concat UART_TX_buffer, GSM_enter
 UART_start_transmit
 delay 82, 43, 77		//delay 1s

 //rcall dbg_TX
 //rcall dbg_RX

 UART_clear_RX_buffer	//Clear RX buffer
 UART_start_receive
 UART_clear_TX_buffer	//Clear TX buffer
 str_concat UART_TX_buffer, GSM_end_msg
 UART_start_transmit 

 cbi PORTD, 7
 /* wait for operation result */
 delay 255,255,255		//long delay
 //UART_stop_receive

 dbg

 str_search UART_RX_buffer, GSM_result
 cpi temp, 1
 brne PC+2
 rjmp GSM_correct_return		////////////////////////////////////////////
 //breq GSM_correct_return
 dbg
 str_search UART_RX_buffer, GSM_ERROR
 cpi temp, 1
 breq GSM_correct_return
 dbg
 rjmp GSM_send_SMS_command
 /* return */
 GSM_correct_return:

 UART_clear_RX_buffer	//Clear TX buffer
 UART_start_receive

 GSM_return:
 UART_start_receive
 sbi PORTD, 7
 delay 255,255,255
 cbi PORTD, 7
 delay 255,255,255
 
 ret
 //.endm

 //------- store strings to SRAM ----------------
 //.macro GSM_init_strings
 GSM_init_strings:
 push YH
 push YL
 push ZH
 push ZL
 push XH
 push XL
 push byte

 ldi YH, high(GSM_new_msg)	//Y is SRAM adress
 ldi YL, low(GSM_new_msg)

 ldi ZH, high(ee_gsm_new_SMS)	//Z is EEPROM adress
 ldi ZL, low(ee_gsm_new_SMS)

 ldi XH,1
 ldi XL,0x8E			

 gsm_EE_read_loop:
 call EEPROM_read_byte		//read byte from EEPROM
 st Y+,byte					//Store byte to SRAM and post-Inc adress
 adiw ZH:ZL, 1				//increment Z register after reading byte

 sbiw XH:XL, 1			//loop things...
 brne gsm_EE_read_loop		//go to EE_read_loop label if r19 is not 0

 pop byte
 pop XL
 pop XH
 pop ZL
 pop ZH
 pop YL
 pop YH
 //.endm
 ret
 //----------------------------------------------
 //======= PROCEDURES ===========================

 //------- time validate ----------------
 GSM_time_validate:
 push YH
 push YL
 push byte

 clr temp

 ld byte, Y+
 cpi byte, '0'
 brlo GSM_time_validate_return_false
 cpi byte, '3'
 brsh GSM_time_validate_return_false

 ld byte, Y+
 cpi byte, '0'
 brlo GSM_time_validate_return_false
 cpi byte, ':'
 brsh GSM_time_validate_return_false

 ld byte, Y+
 cpi byte, ':'
 brne GSM_time_validate_return_false

 ld byte, Y+
 cpi byte, '0'
 brlo GSM_time_validate_return_false
 cpi byte, '7'
 brsh GSM_time_validate_return_false

 ld byte, Y+
 cpi byte, '0'
 brlo GSM_time_validate_return_false
 cpi byte, ':'
 brsh GSM_time_validate_return_false

 ld byte, Y+
 cpi byte, ' '
 brne GSM_time_validate_return_false

 ld byte, Y+
 cpi byte, '0'
 brlo GSM_time_validate_return_false
 cpi byte, '4'
 brsh GSM_time_validate_return_false

 ld byte, Y+
 cpi byte, '0'
 brlo GSM_time_validate_return_false
 cpi byte, ':'
 brsh GSM_time_validate_return_false

 ld byte, Y+
 cpi byte, '/'
 brne GSM_time_validate_return_false

 ld byte, Y+
 cpi byte, '0'
 brlo GSM_time_validate_return_false
 cpi byte, '2'
 brsh GSM_time_validate_return_false

 ld byte, Y+
 cpi byte, '0'
 brlo GSM_time_validate_return_false
 cpi byte, ':'
 brsh GSM_time_validate_return_false

 ld byte, Y+
 cpi byte, '/'
 brne GSM_time_validate_return_false

 ld byte, Y+
 cpi byte, '0'
 brlo GSM_time_validate_return_false
 cpi byte, ':'
 brsh GSM_time_validate_return_false

 ld byte, Y
 cpi byte, '0'
 brlo GSM_time_validate_return_false
 cpi byte, ':'
 brsh GSM_time_validate_return_false

 GSM_time_validate_return_true:
 ldi temp, 1
 GSM_time_validate_return_false:
 pop byte
 pop YL
 pop YH
 ret
//-----------------------------------------------

//------- On/Off validate -----------------------
GSM_OnOff_Validate:
push YH
push YL
push byte

clr temp

ld byte, Y+
cpi byte, 'O'
brne GSM_OnOff_Validate_return_false

ld byte, Y+
cpi byte, 'N'
breq GSM_OnOff_Validate_return_true

cpi byte, 'F'
brne GSM_OnOff_Validate_return_false

ld byte, Y
cpi byte, 'F'
brne GSM_OnOff_Validate_return_false

GSM_OnOff_Validate_return_true:
ldi temp, 1
GSM_OnOff_Validate_return_false:
pop byte
pop YL
pop YH
ret
//-----------------------------------------------
 //==============================================

 dbg_TX:
 push r16
 push r17
 push r18
 push r19
 push YH
 push YL
 push ZH
 push ZL

 in r17, SREG ; store SREG value
 cli

 ldi YH, high(UART_TX_buffer)		 //load to YH register 0
 ldi YL, low(UART_TX_buffer)		 //load to YL 0x60 coz 0x0060 - sram start
 ldi ZH, 0x01
 ldi ZL, 0xFF
 
 ldi r19,0			//r19 is a counter

 EE_write_loop:			//loop 96 times
 ld byte, Y+				//Store byte to SRAM and post-Inc adress
 call EEPROM_write		//read byte from EEPROM
 adiw ZH:ZL,1				//increment Z register after reading byte

 dec r19				//loop things...
 brne EE_write_loop		//go to EE_read_loop label if r19 is not 0

 out SREG, r16 ; restore SREG value (I-bit)

 pop ZL
 pop ZH
 pop YL
 pop YH
 pop r19
 pop r18
 pop r17
 pop r16
 ret

 dbg_RX:
 push r16
 push r17
 push r18
 push r19
 push YH
 push YL
 push ZH
 push ZL

 in r17, SREG ; store SREG value
 cli

 ldi YH, high(UART_RX_buffer)		 //load to YH register 0
 ldi YL, low(UART_RX_buffer)		 //load to YL 0x60 coz 0x0060 - sram start
 ldi ZH, 0x02
 ldi ZL, 0xFF
 
 ldi r19,0			//r19 is a counter

 EE_write1_loop:			//loop 96 times
 ld byte, Y+				//Store byte to SRAM and post-Inc adress
 call EEPROM_write		//read byte from EEPROM
 adiw ZH:ZL,1					//increment Z register after reading byte

 dec r19				//loop things...
 brne EE_write1_loop		//go to EE_read_loop label if r19 is not 0

 out SREG, r16 ; restore SREG value (I-bit)

 pop ZL
 pop ZH
 pop YL
 pop YH
 pop r19
 pop r18
 pop r17
 pop r16
 ret

 EEPROM_write:
 ; Wait for completion of previous write
sbic EECR,EEWE
rjmp EEPROM_write
; Set up address (r18:r17) in address register
out EEARH, ZH
out EEARL, ZL
; Write data (r16) to data register
out EEDR,byte
; Write logical one to EEMWE
sbi EECR,EEMWE
; Start eeprom write by setting EEWE
sbi EECR,EEWE
 ret