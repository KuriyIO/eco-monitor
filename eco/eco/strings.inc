/*
 * strings.inc
 *
 *  Created: 27.03.2016 4:27:29
 *   Author: Hogs
 */ 

 .def counter = r19
 //===== PROCEDURES =======================
 //------ Search ---------------
 str_search_proc:
 // input X (string where need to search)
 // input Y (substring)
 // output R17(0 or 1) 1 is string found, 0 is string not found
 // output R16(byte pointer to first symbol)
 push counter
 push XH
 push XL
 push byte

 clr counter
 clr temp

 str_search_loop:
 ld byte, X
 cpi byte, 0
 breq str_search_return_false

 rcall str_cmp_proc
 cpi temp, 0
 brne str_search_return_true

 inc counter
 adiw XH:XL, 1
 rjmp str_search_loop

 str_search_return_true:
 pop byte
 pop XL
 pop XH
 mov r16, counter
 pop counter
 ldi r17, 1
 ret

 str_search_return_false:
 pop byte
 pop XL
 pop XH
 pop counter
 clr r17
 ret
 //-----------------------------

 //----- String compare --------
 str_cmp_proc:
 //input Y - first (shorter) string
 //input X - second (longer) string
 //output r17 - result (0 - false, 1 - true)
 push XH
 push XL
 push YH
 push YL
 push byte

 str_cmp_loop:
 ld byte, Y
 cpi byte, 0
 breq str_cmp_return_true

 ld temp, X
 cp byte, temp
 brne str_cmp_return_false

 adiw XH:XL, 1
 adiw YH:YL, 1
 rjmp str_cmp_loop

 str_cmp_return_false:
 pop byte
 pop YL
 pop YH
 pop XL
 pop XH
 clr temp
 ret
 str_cmp_return_true:
 pop byte
 pop YL
 pop YH
 pop XL
 pop XH
 ldi temp, 1
 ret
 //-----------------------------

 //-------- str len ------------
 str_len_proc:
 //input X - adress of string
 //output r17 - length
 push byte
 push XH
 push XL
 clr temp

 str_len_loop:
 ld byte, X+
 cpi byte, 0
 breq str_len_return

 inc temp
 rjmp str_len_loop

 str_len_return:
 pop XL
 pop XH
 pop byte
 ret
 //-----------------------------

 //-------- str concat ---------
 str_concat_proc:
 //input X - string 1
 //input Y - string 2
 //no output
 //result is: str1 = str1+str2
 push temp
 push XH
 push XL
 push YH
 push YL

 rcall str_len_proc
 add XL, temp
 clr temp			//set X as end of first string
 adc XH, temp


 str_concat_loop:
 ld temp, Y+
 cp temp, byte
 breq str_concat_return

 st X+, temp
 rjmp str_concat_loop

 str_concat_return:
 pop YL
 pop YH
 pop XL
 pop XH
 pop temp
 ret
 //---------------------------

 //------ Clear string -------
 str_clr_proc:
 //input X - string what need to clear
 //no output

 push temp
 push XH
 push XL

 str_clr_loop:
 ld temp, X+
 cpi temp, 0
 breq str_clr_return

 clr temp
 st X, temp
 rjmp str_clr_loop

 str_clr_return:
 pop XL
 pop XH
 pop temp
 ret
 //---------------------------

 //------ String copy --------
 str_cpy_proc:
 //input X - string which need to copy
 //input Y - string where need to paste
 //input r16 - end of string symbol
 //no output
 push XH
 push XL
 push YH
 push YL
 push temp

 str_cpy_loop:
 ld temp, X+
 cp temp, byte
 breq str_cpy_return

 st Y+, temp
 rjmp str_cpy_loop

 str_cpy_return:
 pop temp
 pop YL
 pop YH
 pop XL
 pop XH
 ret
 //---------------------------

 //------ String copy (using length) ---
 str_cpy_proc_len:
 //input X - string which need to copy
 //input Y - string where need to paste
 //input r16 - count of symbols
 //no output
 push XH
 push XL
 push YH
 push YL
 push temp

 str_cpy_len_loop:
 ld temp, X+
 dec byte
 breq str_cpy_len_return

 st Y+, temp
 rjmp str_cpy_len_loop

 str_cpy_len_return:
 pop temp
 pop YL
 pop YH
 pop XL
 pop XH
 ret
 //---------------------------
 //========================================

 //======= MACRO ==========================
 //----- String search --------------------
 .macro str_search
 // input @0 (string where need to search)
 // input @1 (substring)
 // output R17(0 or 1) 1 is string found, 0 is string not found
 // output R16(byte pointer to first symbol)
 push XH
 push XL
 push YH
 push YL

 ldi XH, high(@0)
 ldi XL, low(@0)
 ldi YH, high(@1)
 ldi YL, low(@1)
 call str_search_proc

 pop YL
 pop YH
 pop XL
 pop XH
 .endm
 //----------------------------------------

 //----- String Compare -------------------
 .macro str_cmp
 push XH
 push XL
 push YH
 push YL

 ldi XH, high(@0)
 ldi XL, low(@0)
 ldi YH, high(@1)
 ldi YL, low(@1)
 rcall str_cmp_proc

 pop YL
 pop YH
 pop XL
 pop XH
 .endm
 //----------------------------------------

 //----- String length --------------------
 .macro str_len
 //input - @0 adress of string
 //output - r17 - length
 push XH
 push XL

 ldi XH, high(@0)
 ldi XL, low(@0)
 rcall str_len_proc

 pop XL
 pop XH
 .endm
 //----------------------------------------

 //----- String concatenation -------------
 .macro str_concat
 //input @0 - adress of first string
 //input @1 - adress of second string
 //no output
 //result is: str1 = str1+str2
 
 push XH
 push XL
 push YH
 push YL
 push byte

 ldi XH, high(@0)
 ldi XL, low(@0)
 ldi YH, high(@1)
 ldi YL, low(@1)
 clr byte

 call str_concat_proc

 pop byte
 pop YL
 pop YH
 pop XL
 pop XH
 .endm
 //----------------------------------------

 //----- Extended string concatenation -------------
 .macro str_concat_ex
 //input @0 - adress of first string
 //input @1 - adress of second string
 //no output
 //result is: str1 = str1+str2
 
 push XH
 push XL
 push YH
 push YL
 push byte

 ldi XH, high(@0)
 ldi XL, low(@0)
 ldi YH, high(@1)
 ldi YL, low(@1)
 ldi byte, @2

 call str_concat_proc

 pop byte
 pop YL
 pop YH
 pop XL
 pop XH
 .endm
 //----------------------------------------

 //------ Clear string --------------------
 .macro str_clr
 //input @0 - string adress what need to clear
 //no output
 push XH
 push XL

 ldi XH, high(@0)
 ldi XL, low(@0)
 rcall str_clr_proc

 pop XL
 pop XH
 .endm
 //----------------------------------------

 //------ string copy ---------------------
 .macro str_cpy
 //input @0 - string which need to copy
 //input @1 - string where need to paste
 //no output
 push XH
 push XL
 push YH
 push YL
 push byte

 ldi XH, high(@0)
 ldi XL, low(@0)
 ldi YH, high(@1)
 ldi YL, low(@1)
 clr byte
 call str_cpy_proc

 pop byte
 pop YL
 pop YH
 pop XL
 pop XH
 .endm
 //----------------------------------------

 //------ extended string copy ------------
 .macro str_cpy_ex
 //input @0 - string which need to copy
 //input @1 - string where need to paste
 //input @2 - end of string symbol
 //no output
 push XH
 push XL
 push YH
 push YL
 push byte

 ldi XH, high(@0)
 ldi XL, low(@0)
 ldi YH, high(@1)
 ldi YL, low(@1)
 ldi byte, @2
 rcall str_cpy_proc

 pop byte
 pop YL
 pop YH
 pop XL
 pop XH
 .endm
 //----------------------------------------

 //------ (using length) string copy ------------
 .macro str_cpy_len
 //input @0 - string which need to copy
 //input @1 - string where need to paste
 //input @2 - count of symbols
 //no output
 push XH
 push XL
 push YH
 push YL
 push byte

 ldi XH, high(@0)
 ldi XL, low(@0)
 ldi YH, high(@1)
 ldi YL, low(@1)
 ldi byte, @2
 rcall str_cpy_proc_len

 pop byte
 pop YL
 pop YH
 pop XL
 pop XH
 .endm
 //----------------------------------------