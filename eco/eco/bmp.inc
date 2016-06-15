/*
 * bmp.inc
 *
 *  Created: 10.06.2016 20:24:10
 *   Author: Hogs
 */ 

 .equ BMPid = 0b11101110


 //======== MACRO =========================

 //-------- BMP init ----------------------
 .macro bmp_init

 /* read calibration coefficients */
 i2c_read_mbyte BMPid, 0xAA, AC1, 22	

 .endm
 //----------------------------------------

 //-------- Get pressure ------------------
 .macro bmp_get_pressure

 call bmp_read_values
 call bmp_calculate
 call bmp_refresh

 .endm
 //----------------------------------------

 //========================================

 //======== PROCEDURES ====================

 //------ Read values ---------------------
 bmp_read_values:

 /* Start temperature measurment */
 ldi temp, 0x2E
 i2c_write_sbyte BMPid, 0xF4, temp

 Wire_delay2 94, 129	//delay 4.5ms

 /* Read temperature value */
 i2c_read_mbyte BMPid, 0xF6, UT, 2

 /* Start pressure measurment */
 ldi temp, 0x34
 i2c_write_sbyte BMPid, 0xF4, temp

 Wire_delay2 94, 129	//delay 4.5ms

 /* Read pressure value */
 i2c_read_mbyte BMPid, 0xF6, UP, 2

 ret
 //----------------------------------------

 //------ Calculate true pressure ---------
 bmp_calculate:

 pusha
 /* Calculate true temperature */

 //X1 = (UT - AC6) * AC5 / (2^15)

 subw UT, AC6, X1+2
 swr AC5, 15, X1
 mulwdw X1+2, X1, X1
 
 //X2 = MC * (2^11) / (X1 + MD)

 swldw MC, 11, X2
 adddww X1, MD, T
 divdw X2, T, X2

 //B5 = X1 + X2

 adddw X1, X2, B5

 //T = (B5 + 8) / (2^4)

 addidw B5, 0x00, 8, T
 sdwr T, 4, T

 /* Calculate true pressure */

 //B6 = B5 - 4000
 subidw B5, 0x0000, 0x0FA0, B6

 //X1 = (B2 * (B6 * B6 / 2^12)) / 2^11
 mulwdw B6+2, B6+2, X1
 sdwr X1, 12, X1
 mulwdw X1+2, B2, X1
 sdwr X1, 11, X1

 //X2 = AC2 * B6 / 2^11
 mulwdw B6+2, AC2, X2
 sdwr X2, 11, X2

 //X3 = X1 + X2
 adddw X1, X2, X3

 //B3 = ((AC1 * 4 + X3) + 2) / 4
 muliw AC1, 4, B3
 adddw B3, X3, B3
 sdwr B3, 2, B3

 //X1 = AC3 * B6 / 2^13
 mulwdw AC3, B6+2, X1
 sdwr X1, 13, X1

 //X2 = (B1 * (B6 * B6 / 2^12)) * 2^16

 mulwdw B6+2, B6+2, X2
 sdwr X2, 12, X2
 mulwdw X2+2, B1, X2
 swldw X2+2, 16, X2

 //X3 = ((X1 + X2) + 2) / 2^2
 adddw X1, X2, X3
 addidw X3, 0x0000, 0x0002, X3
 sdwr X3, 2, X3

 //B4 = AC4 * (X3 + 32768) / 2^15
 addidw X3, 0x0000, 0x8000, B4
 mulwdw B4+2, AC4, B4
 sdwr B4, 15, B4

 //B7 = (UP - B3) * 50000
 subdw UP, B3, B7
 //mulidw B7, 50000, B7

 // if(B7 < 0x80000000)
 //		p = (B7*2) / B4
 // else
 //		p = (B7 / B4) * 2

 //X1 = (P / 2^8) * (P / 2^8)

 //X1 = (X1 * 3098) / 2^16

 //X2 = (-7357 * P) / 2^16

 //p = p + (X1 + X2 + 3791) / 2^4

 /* Convert pressure to mmHg */

 popa
 ret
 //----------------------------------------

 //------ Refresh data in memory ----------
 bmp_refresh:

 ret
 //----------------------------------------

 //------ Test ----------
 //X1 = (long)((UT - AC6)*(float)AC5 / 32768);
 /*LDD R24,Y+15		;Load indirect with displacement 
 LDD R25,Y+16		;Load indirect with displacement 
 MOVW R24,R24		;Copy register pair 
 LDI R26,0x00		;Load immediate 
 LDI R27,0x00		;Load immediate 
 LDD R18,Y+27		;Load indirect with displacement 
 LDD R19,Y+28		;Load indirect with displacement 
 LDD R20,Y+29		;Load indirect with displacement 
 LDD R21,Y+30		;Load indirect with displacement 
 MOVW R14,R18		;Copy register pair 
 MOVW R16,R20		;Copy register pair 
 SUB R14,R24		;Subtract without carry 
 SBC R15,R25		;Subtract with carry 
 SBC R16,R26		;Subtract with carry 
 SBC R17,R27		;Subtract with carry 
 MOVW R26,R16		;Copy register pair 
 MOVW R24,R14		;Copy register pair 
 MOVW R22,R24		;Copy register pair 
 MOVW R24,R26		;Copy register pair 
 CALL 0x0000049A	;Call subroutine 
 MOVW R12,R22		;Copy register pair 
 MOVW R14,R24		;Copy register pair 
 LDD R24,Y+13		;Load indirect with displacement 
 LDD R25,Y+14		;Load indirect with displacement 
 MOVW R24,R24		;Copy register pair 

 LDI R26,0x00		;Load immediate 
 LDI R27,0x00		;Load immediate 
 MOVW R22,R24		;Copy register pair 
 MOVW R24,R26		;Copy register pair 
 CALL 0x00000498	;Call subroutine 
 MOVW R26,R24		;Copy register pair 
 MOVW R24,R22		;Copy register pair 
 MOVW R18,R24		;Copy register pair 
 MOVW R20,R26		;Copy register pair 
 MOVW R24,R14		;Copy register pair 
 MOVW R22,R12		;Copy register pair 
 CALL 0x0000054A	;Call subroutine 
 MOVW R26,R24		;Copy register pair 
 MOVW R24,R22		;Copy register pair 
 LDI R18,0x00		;Load immediate 
 LDI R19,0x00		;Load immediate 
 LDI R20,0x00		;Load immediate 

 LDI R21,0x47		;Load immediate 
 MOVW R22,R24		;Copy register pair 
 MOVW R24,R26		;Copy register pair 
 CALL 0x000003FF	;Call subroutine 
 MOVW R26,R24		;Copy register pair 
 MOVW R24,R22		;Copy register pair 
 MOVW R22,R24		;Copy register pair 
 MOVW R24,R26		;Copy register pair 
 CALL 0x00000467	;Call subroutine 
 MOVW R26,R24		;Copy register pair 
 MOVW R24,R22		;Copy register pair 
 MOVW R22,R24		;Copy register pair 
 MOVW R24,R26		;Copy register pair 
 CALL 0x0000049A	;Call subroutine 
 MOVW R26,R24		;Copy register pair 
 MOVW R24,R22		;Copy register pair 
 STD Y+35,R24		;Store indirect with displacement 

 STD Y+36,R25		;Store indirect with displacement 
 STD Y+37,R26		;Store indirect with displacement 
 STD Y+38,R27		;Store indirect with displacement 

//X2 = (MC * (signed long)2048)/(X1 + MD);	
			
 LDD R24,Y+23		;Load indirect with displacement 
 LDD R25,Y+24		;Load indirect with displacement 
 CLR R26			;Clear Register 
 SBRC R25,7			;Skip if bit in register cleared 
 COM R26			;One's complement 
 MOV R27,R26		;Copy register 
 MOV R0,R23			;Copy register 
 LDI R23,0x0B		;Load immediate 
 LSL R24			;Logical Shift Left 
 ROL R25			;Rotate Left Through Carry 
 ROL R26			;Rotate Left Through Carry 
 ROL R27			;Rotate Left Through Carry 
 DEC R23			;Decrement 
 BRNE PC-0x05		;Branch if not equal 

 MOV R23,R0			;Copy register 
 MOVW R22,R24		;Copy register pair 
 MOVW R24,R26		;Copy register pair 
 CALL 0x0000049A	;Call subroutine 
 MOVW R12,R22		;Copy register pair 
 MOVW R14,R24		;Copy register pair 
 LDD R24,Y+25		;Load indirect with displacement 
 LDD R25,Y+26		;Load indirect with displacement 
 CLR R26			;Clear Register 
 SBRC R25,7			;Skip if bit in register cleared 
 COM R26			;One's complement 
 MOV R27,R26		;Copy register 
 MOVW R22,R24		;Copy register pair 
 MOVW R24,R26		;Copy register pair 
 CALL 0x0000049A	;Call subroutine 
 MOVW R26,R24		;Copy register pair 
 MOVW R24,R22		;Copy register pair 

 LDD R18,Y+35		;Load indirect with displacement 
 LDD R19,Y+36		;Load indirect with displacement 
 LDD R20,Y+37		;Load indirect with displacement 
 LDD R21,Y+38		;Load indirect with displacement 
 MOVW R22,R24		;Copy register pair 
 MOVW R24,R26		;Copy register pair 
 CALL 0x00000397	;Call subroutine 
 MOVW R26,R24		;Copy register pair 
 MOVW R24,R22		;Copy register pair 
 MOVW R18,R24		;Copy register pair 
 MOVW R20,R26		;Copy register pair 
 MOVW R24,R14		;Copy register pair 
 MOVW R22,R12		;Copy register pair 
 CALL 0x000003FF	;Call subroutine 
 MOVW R26,R24		;Copy register pair 
 MOVW R24,R22		;Copy register pair 
 STD Y+39,R24		;Store indirect with displacement 

 STD Y+40,R25		;Store indirect with displacement 
 STD Y+41,R26		;Store indirect with displacement 
 STD Y+42,R27		;Store indirect with displacement 

//B5 = X1 + X2;

 LDD R18,Y+39		;Load indirect with displacement 
 LDD R19,Y+40		;Load indirect with displacement 
 LDD R20,Y+41		;Load indirect with displacement 
 LDD R21,Y+42		;Load indirect with displacement 
 LDD R22,Y+35		;Load indirect with displacement 
 LDD R23,Y+36		;Load indirect with displacement 
 LDD R24,Y+37		;Load indirect with displacement 
 LDD R25,Y+38		;Load indirect with displacement 
 CALL 0x00000397	;Call subroutine 
 MOVW R26,R24		;Copy register pair 
 MOVW R24,R22		;Copy register pair 
 STD Y+43,R24		;Store indirect with displacement 
 STD Y+44,R25		;Store indirect with displacement 
 STD Y+45,R26		;Store indirect with displacement 

 STD Y+46,R27		;Store indirect with displacement 

//T = (B5 + 8) / 16;

 LDI R18,0x00		;Load immediate 
 LDI R19,0x00		;Load immediate 
 LDI R20,0x00		;Load immediate 
 LDI R21,0x41		;Load immediate 
 LDD R22,Y+43		;Load indirect with displacement 
 LDD R23,Y+44		;Load indirect with displacement 
 LDD R24,Y+45		;Load indirect with displacement 
 LDD R25,Y+46		;Load indirect with displacement 
 CALL 0x00000397	;Call subroutine 
 MOVW R26,R24		;Copy register pair 
 MOVW R24,R22		;Copy register pair 
 LDI R18,0x00		;Load immediate 
 LDI R19,0x00		;Load immediate 
 LDI R20,0x80		;Load immediate 
 LDI R21,0x41		;Load immediate 
 MOVW R22,R24		;Copy register pair 

 MOVW R24,R26		;Copy register pair 
 CALL 0x000003FF	;Call subroutine 
 MOVW R26,R24		;Copy register pair 
 MOVW R24,R22		;Copy register pair 
 MOVW R22,R24		;Copy register pair 
 MOVW R24,R26		;Copy register pair 
 CALL 0x00000467	;Call subroutine 
 MOVW R26,R24		;Copy register pair 
 MOVW R24,R22		;Copy register pair 
 STD Y+47,R24		;Store indirect with displacement 
 STD Y+48,R25		;Store indirect with displacement 
 STD Y+49,R26		;Store indirect with displacement 
 STD Y+50,R27		;Store indirect with displacement 

//B6 = (float)B5 - (float)4000;

 LDI R18,0x00		;Load immediate 
 LDI R19,0x00		;Load immediate 
 LDI R20,0x7A		;Load immediate 
 LDI R21,0x45		;Load immediate 

 LDD R22,Y+43		;Load indirect with displacement 
 LDD R23,Y+44		;Load indirect with displacement 
 LDD R24,Y+45		;Load indirect with displacement 
 LDD R25,Y+46		;Load indirect with displacement 
 CALL 0x00000396	;Call subroutine 
 MOVW R26,R24		;Copy register pair 
 MOVW R24,R22		;Copy register pair 
 STD Y+51,R24		;Store indirect with displacement 
 STD Y+52,R25		;Store indirect with displacement 
 STD Y+53,R26		;Store indirect with displacement 
 STD Y+54,R27		;Store indirect with displacement 

//X1 = (B2 * (B6 * B6 / 4096 )) / 2048;
		
 LDD R24,Y+19		;Load indirect with displacement 
 LDD R25,Y+20		;Load indirect with displacement 
 CLR R26			;Clear Register 
 SBRC R25,7			;Skip if bit in register cleared 
 COM R26			;One's complement 
 MOV R27,R26		;Copy register 

 MOVW R22,R24		;Copy register pair 
 MOVW R24,R26		;Copy register pair 
 CALL 0x0000049A	;Call subroutine 
 MOVW R12,R22		;Copy register pair 
 MOVW R14,R24		;Copy register pair 
 LDD R18,Y+51		;Load indirect with displacement 
 LDD R19,Y+52		;Load indirect with displacement 
 LDD R20,Y+53		;Load indirect with displacement 
 LDD R21,Y+54		;Load indirect with displacement 
 LDD R22,Y+51		;Load indirect with displacement 
 LDD R23,Y+52		;Load indirect with displacement 
 LDD R24,Y+53		;Load indirect with displacement 
 LDD R25,Y+54		;Load indirect with displacement 
 CALL 0x0000054A	;Call subroutine 
 MOVW R26,R24		;Copy register pair 
 MOVW R24,R22		;Copy register pair 
 LDI R18,0x00		;Load immediate 

 LDI R19,0x00		;Load immediate 
 LDI R20,0x80		;Load immediate 
 LDI R21,0x45		;Load immediate 
 MOVW R22,R24		;Copy register pair 
 MOVW R24,R26		;Copy register pair 
 CALL 0x000003FF	;Call subroutine 
 MOVW R26,R24		;Copy register pair 
 MOVW R24,R22		;Copy register pair 
 MOVW R18,R24		;Copy register pair 
 MOVW R20,R26		;Copy register pair 
 MOVW R24,R14		;Copy register pair 
 MOVW R22,R12		;Copy register pair 
 CALL 0x0000054A	;Call subroutine 
 MOVW R26,R24		;Copy register pair 
 MOVW R24,R22		;Copy register pair 
 LDI R18,0x00		;Load immediate 
 LDI R19,0x00		;Load immediate 

 LDI R20,0x00		;Load immediate 
 LDI R21,0x45		;Load immediate 
 MOVW R22,R24		;Copy register pair 
 MOVW R24,R26		;Copy register pair 
 CALL 0x000003FF	;Call subroutine 
 MOVW R26,R24		;Copy register pair 
 MOVW R24,R22		;Copy register pair 
 STD Y+35,R24		;Store indirect with displacement 
 STD Y+36,R25		;Store indirect with displacement 
 STD Y+37,R26		;Store indirect with displacement 
 STD Y+38,R27		;Store indirect with displacement 
		
		//X2 = AC2 * B6 / 2048;

 LDD R24,Y+7		;Load indirect with displacement 
 LDD R25,Y+8		;Load indirect with displacement 
 CLR R26			;Clear Register 
 SBRC R25,7			;Skip if bit in register cleared 
 COM R26			;One's complement 
 MOV R27,R26		;Copy register 

 MOVW R22,R24		;Copy register pair 
 MOVW R24,R26		;Copy register pair 
 CALL 0x0000049A	;Call subroutine 
 MOVW R26,R24		;Copy register pair 
 MOVW R24,R22		;Copy register pair 
 LDD R18,Y+51		;Load indirect with displacement 
 LDD R19,Y+52		;Load indirect with displacement 
 LDD R20,Y+53		;Load indirect with displacement 
 LDD R21,Y+54		;Load indirect with displacement 
 MOVW R22,R24		;Copy register pair 
 MOVW R24,R26		;Copy register pair 
 CALL 0x0000054A	;Call subroutine 
 MOVW R26,R24		;Copy register pair 
 MOVW R24,R22		;Copy register pair 
 LDI R18,0x00		;Load immediate 
 LDI R19,0x00		;Load immediate 
 LDI R20,0x00		;Load immediate 

 LDI R21,0x45		;Load immediate 
 MOVW R22,R24		;Copy register pair 
 MOVW R24,R26		;Copy register pair 
 CALL 0x000003FF	;Call subroutine 
 MOVW R26,R24		;Copy register pair 
 MOVW R24,R22		;Copy register pair 
 STD Y+39,R24		;Store indirect with displacement 
 STD Y+40,R25		;Store indirect with displacement 
 STD Y+41,R26		;Store indirect with displacement 
 STD Y+42,R27		;Store indirect with displacement 

//X3 = X1 + X2;

 LDD R18,Y+39		;Load indirect with displacement 
 LDD R19,Y+40		;Load indirect with displacement 
 LDD R20,Y+41		;Load indirect with displacement 
 LDD R21,Y+42		;Load indirect with displacement 
 LDD R22,Y+35		;Load indirect with displacement 
 LDD R23,Y+36		;Load indirect with displacement 
 LDD R24,Y+37		;Load indirect with displacement 

 LDD R25,Y+38		;Load indirect with displacement 
 CALL 0x00000397	;Call subroutine 
 MOVW R26,R24		;Copy register pair 
 MOVW R24,R22		;Copy register pair 
 STD Y+55,R24		;Store indirect with displacement 
 STD Y+56,R25		;Store indirect with displacement 
 STD Y+57,R26		;Store indirect with displacement 
 STD Y+58,R27		;Store indirect with displacement 

		//B3 = ((AC1*4+X3) + 2) / 4;

 LDD R24,Y+5		;Load indirect with displacement 
 LDD R25,Y+6		;Load indirect with displacement 
 LSL R24			;Logical Shift Left 
 ROL R25			;Rotate Left Through Carry 
 LSL R24			;Logical Shift Left 
 ROL R25			;Rotate Left Through Carry 
 CLR R26			;Clear Register 
 SBRC R25,7			;Skip if bit in register cleared 
 COM R26			;One's complement 

 MOV R27,R26		;Copy register 
 MOVW R22,R24		;Copy register pair 
 MOVW R24,R26		;Copy register pair 
 CALL 0x0000049A	;Call subroutine 
 MOVW R26,R24		;Copy register pair 
 MOVW R24,R22		;Copy register pair 
 LDD R18,Y+55		;Load indirect with displacement 
 LDD R19,Y+56		;Load indirect with displacement 
 LDD R20,Y+57		;Load indirect with displacement 
 LDD R21,Y+58		;Load indirect with displacement 
 MOVW R22,R24		;Copy register pair 
 MOVW R24,R26		;Copy register pair 
 CALL 0x00000397	;Call subroutine 
 MOVW R26,R24		;Copy register pair 
 MOVW R24,R22		;Copy register pair 
 LDI R18,0x00		;Load immediate 
 LDI R19,0x00		;Load immediate 

 LDI R20,0x00		;Load immediate 
 LDI R21,0x40		;Load immediate 
 MOVW R22,R24		;Copy register pair 
 MOVW R24,R26		;Copy register pair 
 CALL 0x00000397	;Call subroutine 
 MOVW R26,R24		;Copy register pair 
 MOVW R24,R22		;Copy register pair 
 LDI R18,0x00		;Load immediate 
 LDI R19,0x00		;Load immediate 
 LDI R20,0x80		;Load immediate 
 LDI R21,0x40		;Load immediate 
 MOVW R22,R24		;Copy register pair 
 MOVW R24,R26		;Copy register pair 
 CALL 0x000003FF	;Call subroutine 
 MOVW R26,R24		;Copy register pair 
 MOVW R24,R22		;Copy register pair 
 STD Y+59,R24		;Store indirect with displacement 

 STD Y+60,R25		;Store indirect with displacement 
 STD Y+61,R26		;Store indirect with displacement 
 STD Y+62,R27		;Store indirect with displacement
  
		//X1 = AC3 * B6 / 8192;

 LDD R24,Y+9		;Load indirect with displacement 
 LDD R25,Y+10		;Load indirect with displacement 
 CLR R26			;Clear Register 
 SBRC R25,7			;Skip if bit in register cleared 
 COM R26			;One's complement 
 MOV R27,R26		;Copy register 
 MOVW R22,R24		;Copy register pair 
 MOVW R24,R26		;Copy register pair 
 CALL 0x0000049A	;Call subroutine 
 MOVW R26,R24		;Copy register pair 
 MOVW R24,R22		;Copy register pair 
 LDD R18,Y+51		;Load indirect with displacement 
 LDD R19,Y+52		;Load indirect with displacement 
 LDD R20,Y+53		;Load indirect with displacement 

 LDD R21,Y+54		;Load indirect with displacement 
 MOVW R22,R24		;Copy register pair 
 MOVW R24,R26		;Copy register pair 
 CALL 0x0000054A	;Call subroutine 
 MOVW R26,R24		;Copy register pair 
 MOVW R24,R22		;Copy register pair 
 LDI R18,0x00		;Load immediate 
 LDI R19,0x00		;Load immediate 
 LDI R20,0x00		;Load immediate 
 LDI R21,0x46		;Load immediate 
 MOVW R22,R24		;Copy register pair 
 MOVW R24,R26		;Copy register pair 
 CALL 0x000003FF	;Call subroutine 
 MOVW R26,R24		;Copy register pair 
 MOVW R24,R22		;Copy register pair 
 STD Y+35,R24		;Store indirect with displacement 
 STD Y+36,R25		;Store indirect with displacement 

 STD Y+37,R26		;Store indirect with displacement 
 STD Y+38,R27		;Store indirect with displacement 

//X2 = (B1 * (B6 * B6 / 4096 )) / 65536;

 LDD R24,Y+17		;Load indirect with displacement 
 LDD R25,Y+18		;Load indirect with displacement 
 CLR R26			;Clear Register 
 SBRC R25,7			;Skip if bit in register cleared 
 COM R26			;One's complement 
 MOV R27,R26		;Copy register 
 MOVW R22,R24		;Copy register pair 
 MOVW R24,R26		;Copy register pair 
 CALL 0x0000049A	;Call subroutine 
 MOVW R12,R22		;Copy register pair 
 MOVW R14,R24		;Copy register pair 
 LDD R18,Y+51		;Load indirect with displacement 
 LDD R19,Y+52		;Load indirect with displacement 
 LDD R20,Y+53		;Load indirect with displacement 
 LDD R21,Y+54		;Load indirect with displacement 

 LDD R22,Y+51		;Load indirect with displacement 
 LDD R23,Y+52		;Load indirect with displacement 
 LDD R24,Y+53		;Load indirect with displacement 
 LDD R25,Y+54		;Load indirect with displacement 
 CALL 0x0000054A	;Call subroutine 
 MOVW R26,R24		;Copy register pair 
 MOVW R24,R22		;Copy register pair 
 LDI R18,0x00		;Load immediate 
 LDI R19,0x00		;Load immediate 
 LDI R20,0x80		;Load immediate 
 LDI R21,0x45		;Load immediate 
 MOVW R22,R24		;Copy register pair 
 MOVW R24,R26		;Copy register pair 
 CALL 0x000003FF	;Call subroutine 
 MOVW R26,R24		Copy register pair 
 MOVW R24,R22		Copy register pair 
 MOVW R18,R24		Copy register pair 
--- D:\delme\GccApplication1\GccApplication1\Debug/.././GccApplication1.c ------
 MOVW R20,R26		Copy register pair 
 MOVW R24,R14		Copy register pair 
 MOVW R22,R12		Copy register pair 
 CALL 0x0000054A		Call subroutine 
 MOVW R26,R24		Copy register pair 
 MOVW R24,R22		Copy register pair 
 LDI R18,0x00		Load immediate 
 LDI R19,0x00		Load immediate 
 LDI R20,0x80		Load immediate 
 LDI R21,0x47		Load immediate 
 MOVW R22,R24		Copy register pair 
 MOVW R24,R26		Copy register pair 
 CALL 0x000003FF		Call subroutine 
 MOVW R26,R24		Copy register pair 
 MOVW R24,R22		Copy register pair 
 STD Y+39,R24		Store indirect with displacement 
 STD Y+40,R25		Store indirect with displacement 
--- D:\delme\GccApplication1\GccApplication1\Debug/.././GccApplication1.c ------
 STD Y+41,R26		Store indirect with displacement 
 STD Y+42,R27		Store indirect with displacement 
		X3 = ((X1 + X2) + 2) / 4;
 LDD R18,Y+39		Load indirect with displacement 
 LDD R19,Y+40		Load indirect with displacement 
 LDD R20,Y+41		Load indirect with displacement 
 LDD R21,Y+42		Load indirect with displacement 
 LDD R22,Y+35		Load indirect with displacement 
 LDD R23,Y+36		Load indirect with displacement 
 LDD R24,Y+37		Load indirect with displacement 
 LDD R25,Y+38		Load indirect with displacement 
 CALL 0x00000397		Call subroutine 
 MOVW R26,R24		Copy register pair 
 MOVW R24,R22		Copy register pair 
 LDI R18,0x00		Load immediate 
 LDI R19,0x00		Load immediate 
 LDI R20,0x00		Load immediate 
 LDI R21,0x40		Load immediate 
--- D:\delme\GccApplication1\GccApplication1\Debug/.././GccApplication1.c ------
 MOVW R22,R24		Copy register pair 
 MOVW R24,R26		Copy register pair 
 CALL 0x00000397		Call subroutine 
 MOVW R26,R24		Copy register pair 
 MOVW R24,R22		Copy register pair 
 LDI R18,0x00		Load immediate 
 LDI R19,0x00		Load immediate 
 LDI R20,0x80		Load immediate 
 LDI R21,0x40		Load immediate 
 MOVW R22,R24		Copy register pair 
 MOVW R24,R26		Copy register pair 
 CALL 0x000003FF		Call subroutine 
 MOVW R26,R24		Copy register pair 
 MOVW R24,R22		Copy register pair 
 STD Y+55,R24		Store indirect with displacement 
 STD Y+56,R25		Store indirect with displacement 
 STD Y+57,R26		Store indirect with displacement 
--- D:\delme\GccApplication1\GccApplication1\Debug/.././GccApplication1.c ------
 STD Y+58,R27		Store indirect with displacement 
		B4 = AC4 * (unsigned long)(X3 + 32768) / 32768;
 LDD R24,Y+11		Load indirect with displacement 
 LDD R25,Y+12		Load indirect with displacement 
 MOVW R12,R24		Copy register pair 
 MOV R14,R1		Copy register 
 MOV R15,R1		Copy register 
 LDI R18,0x00		Load immediate 
 LDI R19,0x00		Load immediate 
 LDI R20,0x00		Load immediate 
 LDI R21,0x47		Load immediate 
 LDD R22,Y+55		Load indirect with displacement 
 LDD R23,Y+56		Load indirect with displacement 
 LDD R24,Y+57		Load indirect with displacement 
 LDD R25,Y+58		Load indirect with displacement 
 CALL 0x00000397		Call subroutine 
 MOVW R26,R24		Copy register pair 
 MOVW R24,R22		Copy register pair 
--- D:\delme\GccApplication1\GccApplication1\Debug/.././GccApplication1.c ------
 MOVW R22,R24		Copy register pair 
 MOVW R24,R26		Copy register pair 
 CALL 0x0000046C		Call subroutine 
 MOVW R26,R24		Copy register pair 
 MOVW R24,R22		Copy register pair 
 MOVW R20,R14		Copy register pair 
 MOVW R18,R12		Copy register pair 
 MOVW R22,R24		Copy register pair 
 MOVW R24,R26		Copy register pair 
 CALL 0x000005AD		Call subroutine 
 MOVW R26,R24		Copy register pair 
 MOVW R24,R22		Copy register pair 
 MOV R0,R23		Copy register 
 LDI R23,0x0F		Load immediate 
 LSR R27		Logical shift right 
 ROR R26		Rotate right through carry 
 ROR R25		Rotate right through carry 
--- D:\delme\GccApplication1\GccApplication1\Debug/.././GccApplication1.c ------
 ROR R24		Rotate right through carry 
 DEC R23		Decrement 
 BRNE PC-0x05		Branch if not equal 
 MOV R23,R0		Copy register 
 MOVW R16,R28		Copy register pair 
 SUBI R16,0xC1		Subtract immediate 
 SBCI R17,0xFF		Subtract immediate with carry 
 MOVW R22,R24		Copy register pair 
 MOVW R24,R26		Copy register pair 
 CALL 0x00000498		Call subroutine 
 MOVW R26,R24		Copy register pair 
 MOVW R24,R22		Copy register pair 
 MOVW R30,R16		Copy register pair 
 STD Z+0,R24		Store indirect with displacement 
 STD Z+1,R25		Store indirect with displacement 
 STD Z+2,R26		Store indirect with displacement 
 STD Z+3,R27		Store indirect with displacement 
--- D:\delme\GccApplication1\GccApplication1\Debug/.././GccApplication1.c ------
		B7 = ((unsigned long)UP - B3) * 50000;
 LDD R24,Y+31		Load indirect with displacement 
 LDD R25,Y+32		Load indirect with displacement 
 LDD R26,Y+33		Load indirect with displacement 
 LDD R27,Y+34		Load indirect with displacement 
 MOVW R22,R24		Copy register pair 
 MOVW R24,R26		Copy register pair 
 CALL 0x00000498		Call subroutine 
 MOVW R26,R24		Copy register pair 
 MOVW R24,R22		Copy register pair 
 LDD R18,Y+59		Load indirect with displacement 
 LDD R19,Y+60		Load indirect with displacement 
 LDD R20,Y+61		Load indirect with displacement 
 LDD R21,Y+62		Load indirect with displacement 
 MOVW R22,R24		Copy register pair 
 MOVW R24,R26		Copy register pair 
 CALL 0x00000396		Call subroutine 
 MOVW R26,R24		Copy register pair 
--- D:\delme\GccApplication1\GccApplication1\Debug/.././GccApplication1.c ------
 MOVW R24,R22		Copy register pair 
 MOVW R16,R28		Copy register pair 
 SUBI R16,0xBD		Subtract immediate 
 SBCI R17,0xFF		Subtract immediate with carry 
 LDI R18,0x00		Load immediate 
 LDI R19,0x50		Load immediate 
 LDI R20,0x43		Load immediate 
 LDI R21,0x47		Load immediate 
 MOVW R22,R24		Copy register pair 
 MOVW R24,R26		Copy register pair 
 CALL 0x0000054A		Call subroutine 
 MOVW R26,R24		Copy register pair 
 MOVW R24,R22		Copy register pair 
 MOVW R30,R16		Copy register pair 
 STD Z+0,R24		Store indirect with displacement 
 STD Z+1,R25		Store indirect with displacement 
 STD Z+2,R26		Store indirect with displacement 
--- D:\delme\GccApplication1\GccApplication1\Debug/.././GccApplication1.c ------
 STD Z+3,R27		Store indirect with displacement 
		if (B7 < 0x80000000) 
 MOVW R24,R28		Copy register pair 
 SUBI R24,0xBD		Subtract immediate 
 SBCI R25,0xFF		Subtract immediate with carry 
 LDI R18,0x00		Load immediate 
 LDI R19,0x00		Load immediate 
 LDI R20,0x00		Load immediate 
 LDI R21,0x4F		Load immediate 
 MOVW R30,R24		Copy register pair 
 LDD R22,Z+0		Load indirect with displacement 
 LDD R23,Z+1		Load indirect with displacement 
 LDD R24,Z+2		Load indirect with displacement 
 LDD R25,Z+3		Load indirect with displacement 
 CALL 0x000003FB		Call subroutine 
 TST R24		Test for Zero or Minus 
 BRGE PC+0x24		Branch if greater or equal, signed 
			p = (B7 * 2) / B4;
 MOVW R24,R28		Copy register pair 
--- D:\delme\GccApplication1\GccApplication1\Debug/.././GccApplication1.c ------
 SUBI R24,0xBD		Subtract immediate 
 SBCI R25,0xFF		Subtract immediate with carry 
 MOVW R30,R24		Copy register pair 
 LDD R24,Z+0		Load indirect with displacement 
 LDD R25,Z+1		Load indirect with displacement 
 LDD R26,Z+2		Load indirect with displacement 
 LDD R27,Z+3		Load indirect with displacement 
 MOVW R18,R24		Copy register pair 
 MOVW R20,R26		Copy register pair 
 MOVW R22,R24		Copy register pair 
 MOVW R24,R26		Copy register pair 
 CALL 0x00000397		Call subroutine 
 MOVW R26,R24		Copy register pair 
 MOVW R24,R22		Copy register pair 
 MOVW R18,R28		Copy register pair 
 SUBI R18,0xC1		Subtract immediate 
 SBCI R19,0xFF		Subtract immediate with carry 
--- D:\delme\GccApplication1\GccApplication1\Debug/.././GccApplication1.c ------
 MOVW R30,R18		Copy register pair 
 LDD R18,Z+0		Load indirect with displacement 
 LDD R19,Z+1		Load indirect with displacement 
 LDD R20,Z+2		Load indirect with displacement 
 LDD R21,Z+3		Load indirect with displacement 
 MOVW R22,R24		Copy register pair 
 MOVW R24,R26		Copy register pair 
 CALL 0x000003FF		Call subroutine 
 MOVW R26,R24		Copy register pair 
 MOVW R24,R22		Copy register pair 
 STD Y+1,R24		Store indirect with displacement 
 STD Y+2,R25		Store indirect with displacement 
 STD Y+3,R26		Store indirect with displacement 
 STD Y+4,R27		Store indirect with displacement 
 RJMP PC+0x0021		Relative jump 
			p = (B7 / B4) * 2;
 MOVW R24,R28		Copy register pair 
 SUBI R24,0xBD		Subtract immediate 
--- D:\delme\GccApplication1\GccApplication1\Debug/.././GccApplication1.c ------
 SBCI R25,0xFF		Subtract immediate with carry 
 MOVW R18,R28		Copy register pair 
 SUBI R18,0xC1		Subtract immediate 
 SBCI R19,0xFF		Subtract immediate with carry 
 MOVW R30,R18		Copy register pair 
 LDD R18,Z+0		Load indirect with displacement 
 LDD R19,Z+1		Load indirect with displacement 
 LDD R20,Z+2		Load indirect with displacement 
 LDD R21,Z+3		Load indirect with displacement 
 MOVW R30,R24		Copy register pair 
 LDD R22,Z+0		Load indirect with displacement 
 LDD R23,Z+1		Load indirect with displacement 
 LDD R24,Z+2		Load indirect with displacement 
 LDD R25,Z+3		Load indirect with displacement 
 CALL 0x000003FF		Call subroutine 
 MOVW R26,R24		Copy register pair 
 MOVW R24,R22		Copy register pair 
--- D:\delme\GccApplication1\GccApplication1\Debug/.././GccApplication1.c ------
 MOVW R18,R24		Copy register pair 
 MOVW R20,R26		Copy register pair 
 MOVW R22,R24		Copy register pair 
 MOVW R24,R26		Copy register pair 
 CALL 0x00000397		Call subroutine 
 MOVW R26,R24		Copy register pair 
 MOVW R24,R22		Copy register pair 
 STD Y+1,R24		Store indirect with displacement 
 STD Y+2,R25		Store indirect with displacement 
 STD Y+3,R26		Store indirect with displacement 
 STD Y+4,R27		Store indirect with displacement 
		X1 = (p / 256 ) * (p / 256 );
 LDI R18,0x00		Load immediate 
 LDI R19,0x00		Load immediate 
 LDI R20,0x80		Load immediate 
 LDI R21,0x43		Load immediate 
 LDD R22,Y+1		Load indirect with displacement 
 LDD R23,Y+2		Load indirect with displacement 
--- D:\delme\GccApplication1\GccApplication1\Debug/.././GccApplication1.c ------
 LDD R24,Y+3		Load indirect with displacement 
 LDD R25,Y+4		Load indirect with displacement 
 CALL 0x000003FF		Call subroutine 
 MOVW R26,R24		Copy register pair 
 MOVW R24,R22		Copy register pair 
 MOVW R12,R24		Copy register pair 
 MOVW R14,R26		Copy register pair 
 LDI R18,0x00		Load immediate 
 LDI R19,0x00		Load immediate 
 LDI R20,0x80		Load immediate 
 LDI R21,0x43		Load immediate 
 LDD R22,Y+1		Load indirect with displacement 
 LDD R23,Y+2		Load indirect with displacement 
 LDD R24,Y+3		Load indirect with displacement 
 LDD R25,Y+4		Load indirect with displacement 
 CALL 0x000003FF		Call subroutine 
 MOVW R26,R24		Copy register pair 
--- D:\delme\GccApplication1\GccApplication1\Debug/.././GccApplication1.c ------
 MOVW R24,R22		Copy register pair 
 MOVW R18,R24		Copy register pair 
 MOVW R20,R26		Copy register pair 
 MOVW R24,R14		Copy register pair 
 MOVW R22,R12		Copy register pair 
 CALL 0x0000054A		Call subroutine 
 MOVW R26,R24		Copy register pair 
 MOVW R24,R22		Copy register pair 
 STD Y+35,R24		Store indirect with displacement 
 STD Y+36,R25		Store indirect with displacement 
 STD Y+37,R26		Store indirect with displacement 
 STD Y+38,R27		Store indirect with displacement 
		X1 = (X1 * 3038) / 65536;
 LDI R18,0x00		Load immediate 
 LDI R19,0xE0		Load immediate 
 LDI R20,0x3D		Load immediate 
 LDI R21,0x45		Load immediate 
 LDD R22,Y+35		Load indirect with displacement 
--- D:\delme\GccApplication1\GccApplication1\Debug/.././GccApplication1.c ------
 LDD R23,Y+36		Load indirect with displacement 
 LDD R24,Y+37		Load indirect with displacement 
 LDD R25,Y+38		Load indirect with displacement 
 CALL 0x0000054A		Call subroutine 
 MOVW R26,R24		Copy register pair 
 MOVW R24,R22		Copy register pair 
 LDI R18,0x00		Load immediate 
 LDI R19,0x00		Load immediate 
 LDI R20,0x80		Load immediate 
 LDI R21,0x47		Load immediate 
 MOVW R22,R24		Copy register pair 
 MOVW R24,R26		Copy register pair 
 CALL 0x000003FF		Call subroutine 
 MOVW R26,R24		Copy register pair 
 MOVW R24,R22		Copy register pair 
 STD Y+35,R24		Store indirect with displacement 
 STD Y+36,R25		Store indirect with displacement 
--- D:\delme\GccApplication1\GccApplication1\Debug/.././GccApplication1.c ------
 STD Y+37,R26		Store indirect with displacement 
 STD Y+38,R27		Store indirect with displacement 
		X2 = (-7357 * p) / 65536;
 LDI R18,0x00		Load immediate 
 LDI R19,0xE8		Load immediate 
 LDI R20,0xE5		Load immediate 
 LDI R21,0xC5		Load immediate 
 LDD R22,Y+1		Load indirect with displacement 
 LDD R23,Y+2		Load indirect with displacement 
 LDD R24,Y+3		Load indirect with displacement 
 LDD R25,Y+4		Load indirect with displacement 
 CALL 0x0000054A		Call subroutine 
 MOVW R26,R24		Copy register pair 
 MOVW R24,R22		Copy register pair 
 LDI R18,0x00		Load immediate 
 LDI R19,0x00		Load immediate 
 LDI R20,0x80		Load immediate 
 LDI R21,0x47		Load immediate 
--- D:\delme\GccApplication1\GccApplication1\Debug/.././GccApplication1.c ------
 MOVW R22,R24		Copy register pair 
 MOVW R24,R26		Copy register pair 
 CALL 0x000003FF		Call subroutine 
 MOVW R26,R24		Copy register pair 
 MOVW R24,R22		Copy register pair 
 STD Y+39,R24		Store indirect with displacement 
 STD Y+40,R25		Store indirect with displacement 
 STD Y+41,R26		Store indirect with displacement 
 STD Y+42,R27		Store indirect with displacement 
		p = p + (X1 + X2 + 3791) / 16;
 LDD R18,Y+39		Load indirect with displacement 
 LDD R19,Y+40		Load indirect with displacement 
 LDD R20,Y+41		Load indirect with displacement 
 LDD R21,Y+42		Load indirect with displacement 
 LDD R22,Y+35		Load indirect with displacement 
 LDD R23,Y+36		Load indirect with displacement 
 LDD R24,Y+37		Load indirect with displacement 
 LDD R25,Y+38		Load indirect with displacement 
--- D:\delme\GccApplication1\GccApplication1\Debug/.././GccApplication1.c ------
 CALL 0x00000397		Call subroutine 
 MOVW R26,R24		Copy register pair 
 MOVW R24,R22		Copy register pair 
 LDI R18,0x00		Load immediate 
 LDI R19,0xF0		Load immediate 
 LDI R20,0x6C		Load immediate 
 LDI R21,0x45		Load immediate 
 MOVW R22,R24		Copy register pair 
 MOVW R24,R26		Copy register pair 
 CALL 0x00000397		Call subroutine 
 MOVW R26,R24		Copy register pair 
 MOVW R24,R22		Copy register pair 
 LDI R18,0x00		Load immediate 
 LDI R19,0x00		Load immediate 
 LDI R20,0x80		Load immediate 
 LDI R21,0x41		Load immediate 
 MOVW R22,R24		Copy register pair 
--- D:\delme\GccApplication1\GccApplication1\Debug/.././GccApplication1.c ------
 MOVW R24,R26		Copy register pair 
 CALL 0x000003FF		Call subroutine 
 MOVW R26,R24		Copy register pair 
 MOVW R24,R22		Copy register pair 
 MOVW R18,R24		Copy register pair 
 MOVW R20,R26		Copy register pair 
 LDD R22,Y+1		Load indirect with displacement 
 LDD R23,Y+2		Load indirect with displacement 
 LDD R24,Y+3		Load indirect with displacement 
 LDD R25,Y+4		Load indirect with displacement 
 CALL 0x00000397		Call subroutine 
 MOVW R26,R24		Copy register pair 
 MOVW R24,R22		Copy register pair 
 STD Y+1,R24		Store indirect with displacement 
 STD Y+2,R25		Store indirect with displacement 
 STD Y+3,R26		Store indirect with displacement 
 STD Y+4,R27		Store indirect with displacement*/
 //----------------------
 //========================================
