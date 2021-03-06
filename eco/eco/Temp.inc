/*
 * Temp.inc
 *
 *  Created: 31.05.2016 1:35:52
 *   Author: Hogs
 */

.equ Wire_DDR = DDRA // ������� ����� �������
.equ Wire_PORT = PORTA // ������� ����� �������
.equ Wire_PIN = PINA // ������� ����� �������
.equ Wire_BIT = 0 // ��� ����� �� ������� ������ 


 //========== MACRO =============================
 //---------- Wire delay 1 ----------------------
 .macro Wire_delay1
 ldi  r18, @0
 rcall Wire_delay1_proc
 .endm
 //----------------------------------------------

 //---------- Wire delay 2 ----------------------
 .macro Wire_delay2
 ldi  r18, @0
 ldi  r19, @1
 rcall Wire_delay2_proc
 .endm
 //----------------------------------------------
 //---------- Write byte ------------------------
 .macro Wire_write
 ldi byte, @0
 rcall Wire_Write_proc
 .endm
 //----------------------------------------------

 //---------- Check reset -----------------------
 .macro Wire_reset
 rcall Wire_Reset_proc
 tst temp
 breq wire_return
 .endm
 //----------------------------------------------

 //---------- Main ------------------------------
 //.macro Temperature
 Temperature:
 rcall Wire_Reset_proc
 tst temp
 breq wire_return
 rcall Wire_Init

 rcall Wire_Reset_proc
 tst temp
 breq wire_return
 rcall Wire_Measure

 delay 82, 43, 77		//delay 1s

 rcall Wire_Reset_proc
 tst temp
 breq wire_return
 rcall Wire_ReadTemp

 rcall Wire_Reset_proc
 tst temp
 breq wire_return

 lds byte, Wire_temp+1
 swap byte
 andi byte, 0xF0 
 lds temp, Wire_temp
 swap temp
 andi temp, 0x0F
 or byte, temp

 lds temp, Wire_temp+1
 sbrs temp, 7
 rjmp wire_temp_plus

 ldi temp, '-'
 sts temperature_value, temp
 com byte
 inc byte
 rjmp wire_refresh

 wire_temp_plus:
 ldi temp, ' '
 sts temperature_value, temp

 wire_refresh:
 clr temp
 
 cpi byte, 10
 brlo wire_lbl
 inc temp
 subi byte, 10
 rjmp PC-4 
 wire_lbl:

 push r18
 ldi r18, 48
 add temp, r18
 add byte, r18
 pop r18
 sts temperature_value+1, temp
 sts temperature_value+2, byte

 wire_return:
 //.endm
 ret
 //----------------------------------------------
 //==============================================

 //========== PROCEDURES ========================
 //---------- Check line -----------------
 Wire_Reset_proc:
 sbi Wire_DDR, Wire_BIT			// ���� �� �����
 cbi Wire_PORT, Wire_BIT			// ������������ ����� �� �����
 Wire_delay2 10, 249				// �������� 480 ���, ��� ������
 cbi Wire_DDR, Wire_BIT			// ���� �� ����
 Wire_delay2 2, 115				// ���� ���� ���� 70 ���
 sbis Wire_PIN, Wire_BIT		// ���������� ��������� ������, ���� ��� ����� � 1
 ldi temp, 1					// � ��������� ���������� ������� � 1
 sbic Wire_PIN, Wire_BIT		// ���������� ��������� ������, ���� ��� ����� � 0
 clr temp						// � ��������� ���������� ������� � 0
 Wire_delay2 9, 132				// ���� ���� ���� 410 ���
 ret
 //---------------------------------------

 //--------- Set 12bit mode --------------
 Wire_Init:  
 Wire_write 0xCC	// ���������� ���������� ����� �������
 Wire_write 0x4E	// ������� ��� ������ ����� ������ � RAM �������� �������
 Wire_write 0xFF	// 0xFF ���������� � ������ 2 ��������, ��� �������� �����������, �� ��� �� 
 Wire_write 0xFF	// �����, ������� �� ��������� � ����������� ���������
 Wire_write 0x7F	// � ��� ��� ������� ��� �������� 12 ��� - 7F
 ret
 //---------------------------------------

 //--------- Start measuring -------------
 Wire_Measure:
 Wire_write 0xCC	// ���������� ���������� ����� �������
 Wire_write 0x44	// ������� ��� ���� �� ��������������� �����������
 ret
 //---------------------------------------

 //--------- Read Temperature ------------
 Wire_ReadTemp:
 Wire_write 0xCC		// ������ ������� 0xCC, ��� ���������� ���������� ����� �������
 Wire_write 0xBE		// ������� �������, ��� �� ������ ����� ������
 rcall Wire_Read_proc	// � ��� ��� �������� ������, ��������� ������
 sts Wire_temp, byte	// � �������� ��� � ������, �� ����� Trm
 rcall Wire_Read_proc	// ������ ������
 sts Wire_temp+1, byte	// � �������� ��� � ������, �� ����� Trm+1
 ret
 //---------------------------------------

 //---------- Read byte ------------------
 Wire_Read_proc:
 ldi temp, 8
 clr byte
 wire_read_bit:
 sbi Wire_DDR, Wire_BIT		// ����� �� �����
 cbi Wire_PORT, Wire_BIT	// ������������ ����� �� �����
 Wire_delay1 32				// ���� 6 �����������
 cbi Wire_DDR, Wire_BIT		// ����� �� ����
 Wire_delay1 48 			// ���� 9 �����������
 sbis Wire_PIN, Wire_BIT
 clc						// ������� ��� C = 0
 sbic Wire_PIN, Wire_BIT
 sec						// ������� ��� C = 1
 ror byte					// ���������� ����������� ����� ������ ����� �
 Wire_delay2 2, 35			// ���� 55 �����������
 dec temp
 brne wire_read_bit
 ret
 //---------------------------------------

 //---------- Write byte -----------------
 Wire_Write_proc:
 ldi temp, 8
 wire_write_bit: 
 sbi Wire_DDR, Wire_BIT			// ����� �� �����
 cbi Wire_PORT, Wire_BIT		// ������������ ����� �� �����
 sbrc byte, 0					// ��������, � r16 ��� 0 ������ ��� ���������� 
 rjmp wire_write_1				// ���� ���������� �������� �� ���� �����

 Wire_delay2 2, 62				// ���� 60 �����������
 cbi Wire_DDR, Wire_BIT			// ����� �� ����
 Wire_delay1 53					// ���� 10 �����������
 rjmp wire_next_bit

 wire_write_1:
 Wire_delay1 32					// ���� 6 �����������
 cbi Wire_DDR, Wire_BIT			// ����� �� ����
 Wire_delay2 2, 83					// ���� 64 �����������

 wire_next_bit:
 lsr byte
 dec temp
 brne wire_write_bit
 ret
 //---------------------------------------

 //--------- Short delay -----------------
 Wire_delay2_proc:
 dec  r19
 brne Wire_delay2_proc
 dec  r18
 brne Wire_delay2_proc
 ret
 //---------------------------------------

 //--------- Short delay -----------------
 Wire_delay1_proc:
 dec  r18
 brne Wire_delay1_proc
 ret
 //---------------------------------------
 //==============================================