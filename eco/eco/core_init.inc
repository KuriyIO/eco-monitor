/*
 * Clear all registers and SRAM
 * Initialization stack
 */ 

//================SRAM CLEAR=================================
RAM_Flush:	
LDI	ZL,Low(SRAM_START)
LDI	ZH,High(SRAM_START)
CLR	R16
Flush:
ST Z+,R16
CPI	ZH,High(RAMEND+1)
BRNE Flush
CPI	ZL,Low(RAMEND+1)	
BRNE Flush 
CLR	ZL
CLR	ZH
//===============REGISTERS CLEAR=============================
LDI	ZL, 30			
CLR	ZH		
DEC	ZL		
ST Z,ZH		
BRNE PC-2		
//===============STACK INIT==================================
LDI R16,Low(RAMEND)		
OUT SPL,R16			
LDI R16,High(RAMEND)
OUT SPH,R16
//===========================================================