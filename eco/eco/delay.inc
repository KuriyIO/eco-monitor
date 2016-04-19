/*
 * delay.inc
 *
 *  Created: 30.03.2016 0:23:08
 *   Author: Hogs
 */ 


//========= PROCEDURES ======================================
//------delay proc-------------------------------------------
      
 delay_proc:

delay_proc_label: 
	dec  r25
    brne delay_proc_label
    dec  r24
    brne delay_proc_label
	dec  r23
    brne delay_proc_label

 ret

//======== MACRO ===========================================  
//----------delay macro-------------------------------------
  
 .macro delay
 push r25
 push r24
 push r23

 ldi r25,@0
 ldi r24,@1
 ldi r23,@2

 call delay_proc

 pop r23
 pop r24
 pop r25
 .endm
//==========================================================