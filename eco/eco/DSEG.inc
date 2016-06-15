/*
 * DSEG.inc
 * Data segment
 *  Created: 26.03.2016 0:38:52
 *   Author: Hogs
 */ 

// In this segment we can to reserve some memory
// for strings for example...
.dseg
.org 0x0060							//starting from 0x0060 because 
									//before that adress we have reserved memory for registers

pressure_msg:			.byte 16	//Pressure string
temperature_msg:		.byte 16	//Temperature message string
time_msg:				.byte 16	//Time message string
mc_msg:					.byte 16	//Memory Card option message string
sms_msg:				.byte 16	//SMS option message string
sound_msg:				.byte 16	//Sound option message string
pressure_value:			.byte 16	//string which contain pressure value
temperature_value:		.byte 16	//string which contain temperature value
time_value:				.byte 16	//string which contain time value
mc_value:				.byte 16	//string which contain memory card On/Off value
sms_value:				.byte 16	//string which contain SMS On/Off value
sound_value:			.byte 16	//string which contain sound On/Off value
menu_location:			.byte 1		//current menu location
tick_count:				.byte 1		//count of button activations
last_btn:				.byte 1		//number of last pressed button
sound_status:			.byte 1		//status of sound
I2C_status:				.byte 1		//I2C status register
I2C_buffer:				.byte 5		//I2C buffer
I2C_device:				.byte 1		//I2C device adress + R/W
I2C_data_pointer:		.byte 1		//pointer to clock's register
I2C_data_read:			.byte 1		//readed data
I2C_data_write:			.byte 1		//data which need to write
time_set_status:		.byte 1		//time set status register
time_set_value:			.byte 1		//contain a value which need to set
time_set_ranges:		.byte 10	//contain a table with max values range
time_set_max_values:	.byte 5		//contain a max values of hh:mm dd/MM/yy
time_set_buffer:		.byte 2		//contain a active pair values
time_set_blink_count:	.byte 1		//count of tick before blink
UART_TX_buffer:			.byte 256	//UART Transmit buffer
UART_TX_end:			.byte 1		//end of tx string
UART_RX_buffer:			.byte 256	//UART Receive buffer
UART_RX_end:			.byte 1		//end of rx string
UART_TX_byte_p:			.byte 1		//pointer to Transmit buffer
UART_RX_byte_p:			.byte 1		//pointer to Receive buffer
UART_Status:			.byte 1		//UART Status register
GSM_Status:				.byte 1		//GSM status register
GSM_new_msg:			.byte 13	//check new message string
GSM_read_SMS1:			.byte 9		//read SMS string part 1
GSM_read_SMS2:			.byte 3		//read SMS string part 2
GSM_result:				.byte 3		// "OK" string
GSM_write:				.byte 9		//send message string
GSM_write_msg:			.byte 2		//">" symbol
GSM_enter:				.byte 2		//send 0x0D
GSM_end_msg:			.byte 2		//send 0x1A
GSM_phone_start:		.byte 3		// ", string
GSM_get_cmd:			.byte 11	//"device get" string
GSM_set_cmd:			.byte 11	//"device set" string
GSM_info_parameter:		.byte 5		//"info" string
GSM_time_parameter:		.byte 5		//"time" string
GSM_temp_parameter:		.byte 5		//"temp" string
GSM_pressure_parameter:	.byte 9		//"pressure" string
GSM_sound_parameter:	.byte 6		//"sound" string
GSM_memory_parameter:	.byte 7		//"memory" string
GSM_invalid_get:		.byte 84	//invalid get message
GSM_invalid_set:		.byte 65	//invalid set message
On_str:					.byte 4		//"On " string
Off_str:				.byte 4		//"Off" string
GSM_invalid_time_msg:	.byte 53	//invalid time set message
GSM_invalid_OnOff:		.byte 69	//invalid On/Off set message
GSM_ERROR:				.byte 6		//"ERROR" string
GSM_CSQ:				.byte 8
GSM_sms_index:			.byte 3		//index of message
GSM_phone_number:		.byte 15	//phone number string
end_of_string_phone:	.byte 1		//'\0' string
MC_Settings:			.byte 1		//memory card settings (On/Off)
Sound_Settings:			.byte 1		//sound settings (On/Off)
Wire_temp:				.byte 2		//data from 1-Wire DS18B20
temp_buffer:			.byte 10

//- - - - - BMP parameters - - - - - - - 

// Calibration coefficients
AC1:		.byte 2
AC2:		.byte 2
AC3:		.byte 2
AC4:		.byte 2
AC5:		.byte 2
AC6:		.byte 2
B1:			.byte 2
B2:			.byte 2
MB:			.byte 2
MC:			.byte 2
MD:			.byte 2

// Temperature and pressure values
UT:			.byte 2
UP:			.byte 2

// Calculation data
X1:			.byte 4
X2:			.byte 4
B5:			.byte 4
T:			.byte 4

B6:			.byte 4
X3:			.byte 4
B3:			.byte 4
B4:			.byte 4
B7:			.byte 4
P:			.byte 4
//- - - - - - - - - - - - - - - - - - - -

testtest:				.byte 1	