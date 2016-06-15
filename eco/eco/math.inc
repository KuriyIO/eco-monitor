/*
 * math.inc
 *
 *  Created: 10.06.2016 22:28:53
 *   Author: Hogs
 */ 

 //======= MACRO ==========================

 //------- sub word -----------------------
 // subw A, B, C
 // C = A - B
 // A, B, C - is a pointers to a word

 .macro subw

 lds r16, @0
 lds r17, @0 +1
 lds r18, @1
 lds r19, @1 +1

 sub r17, r19
 sbc r16, r18

 sts @2, r16
 sts @2 +1, r17

 .endm
 //----------------------------------------

 //------- sub const word -----------------
 // subiw A, K, C
 // C = A - K
 // A, C - is a pointers to a word
 // K - is number

 .macro subiw

 lds r16, @0
 lds r17, @0 +1
 ldi r18, high(@1)
 ldi r19, low(@1)

 sub r17, r19
 sbc r16, r18

 sts @2, r16
 sts @2 +1, r17

 .endm
 //----------------------------------------

 //-------- add word ----------------------
 // addw A, B, C
 // C = A + B
 // A, B, C - is a pointers to a word

 .macro addw

 lds r16, @0
 lds r17, @0 +1
 lds r18, @1
 lds r19, @1 +1

 add r17, r19
 adc r16, r18

 sts @2, r16
 sts @2 +1, r17

 .endm
 //----------------------------------------

 //-------- add const word ----------------
 // addiw A, K, C
 // C = A + K
 // A, C - is a pointers to a word
 // K - is a number

 .macro addiw

 lds r16, @0
 lds r17, @0 +1
 ldi r18, high(@1)
 ldi r19, low(@1)

 add r17, r19
 adc r16, r18

 sts @2, r16
 sts @2 +1, r17

 .endm
 //----------------------------------------

 //-------- mul word ----------------------
 // mulw A, B, C
 // C = A * B
 // A, B, C - is a pointers to a word

 .macro mulw

 lds r16, @0
 lds r17, @0 +1
 mov r18, r16
 mov r19, r17
 lds r20, @1
 lds r21, @1 +1

 add r17, r19
 adc r16, r18
 sbiw r20:r21, 1
 brne PC-3

 sts @2, r16
 sts @2 +1, r17

 .endm
 //----------------------------------------

 //-------- mul const word ----------------
 // muliw A, K, C
 // C = A * K
 // A, C - is a pointers to a word
 // K - is a number

 .macro muliw

 lds r16, @0
 lds r17, @0 +1
 ldi r18, high(@1)
 ldi r19, low(@1)
 clr r20
 clr r21
 clr r22
 clr r23
 clr r24

 clt
 tst r16
 brpl muliw_2
 com r16
 com r17
 ldi r30, 1
 add r17, r30
 clr r30
 adc r16, r30
 set

 muliw_2:
 tst r18
 brpl muliw_lbl
 com r18
 com r19
 ldi r30, 1
 add r19, r30
 clr r30
 adc r18, r30

 brtc PC+3
 clt
 rjmp PC+2
 set

 muliw_lbl:
 mul r16, r18
 mov r20, r1
 mov r21, r0
 mul r17, r18
 mov r22, r0
 add r21, r1
 adc r20, r24
 mul r16, r19
 add r22, r0
 adc r21, r1
 adc r20, r24
 mul r17, r19
 mov r23, r0
 add r22, r1
 adc r21, r24
 adc r20, r24

 brtc muliw_ret
 tst r20
 brne PC+3
 ser r20
 rjmp PC+3
 neg r20
 dec r20

 tst r21
 brne PC+3
 ser r21
 rjmp PC+3
 neg r21
 dec r21

 tst r22
 brne PC+3
 ser r22
 rjmp PC+3
 neg r22
 dec r22

 tst r23
 brne PC+3
 ser r23
 rjmp PC+3
 neg r23 
 dec r23

 ldi r30, 1
 add r23, r30
 clr r30
 adc r22, r30
 adc r21, r30
 adc r20, r30

 muliw_ret:
 sts @2, r20
 sts @2 +1, r21
 sts @2 +2, r22
 sts @2 +3, r23

 .endm
 //----------------------------------

 //-------- div word ----------------
 // divw A, B, C
 // C = A / B
 // A, B, C - is a pointers to a word

 .macro divw

 lds r16, @0
 lds r17, @0 +1
 lds r18, @1
 lds r19, @1 +1
 clr r20
 clr r21 

 adiw r20:r21, 1
 sub r17, r19
 sbc r16, r18
 brpl PC-3

 sts @2, r20
 sts @2 +1, r21

 .endm
 //----------------------------------

 //-------- div const word ----------------
 // diviw A, K, C
 // C = A / K
 // A, C - is a pointers to a word
 // K - is a number

 .macro diviw

 lds r16, @0
 lds r17, @0 +1
 ldi r18, high(@1)
 ldi r19, low(@1)
 clr XH
 clr XL 

 adiw XH:XL, 1
 sub r17, r19
 sbc r16, r18
 brpl PC-3

 sts @2, r20
 sts @2 +1, r21

 .endm
 //----------------------------------

 //------- sub double word -----------------------
 // subdw A, B, C
 // C = A - B
 // A, B, C - is a pointers to a double word

 .macro subdw

 lds r16, @0
 lds r17, @0 +1
 lds r18, @0 +2
 lds r19, @0 +3
 lds r20, @1
 lds r21, @1 +1
 lds r22, @1 +2
 lds r23, @1 +3

 sub r19, r23
 sbc r18, r22
 sbc r17, r21
 sbc r16, r20

 sts @2, r16
 sts @2 +1, r17
 sts @2 +2, r18
 sts @2 +3, r19

 .endm
 //----------------------------------------

 //------- sub const double word -----------------
 // subidw A, Kh, Kl, C
 // C = A - Kh:Kl
 // A, C - is a pointers to a double word
 // Kh - is high word number
 // Kl - is low word number

 .macro subidw

 lds r16, @0
 lds r17, @0 +1
 lds r18, @0 +2
 lds r19, @0 +3
 ldi r20, high(@1)
 ldi r21, low(@1)
 ldi r22, high(@2)
 ldi r23, low(@2)

 sub r19, r23
 sbc r18, r22
 sbc r17, r21
 sbc r16, r20

 sts @3, r16
 sts @3 +1, r17
 sts @3 +2, r18
 sts @3 +3, r19

 .endm
 //----------------------------------------

 //-------- add double word ----------------------
 // adddw A, B, C
 // C = A + B
 // A, B, C - is a pointers to a double word

 .macro adddw

 lds r16, @0
 lds r17, @0 +1
 lds r18, @0 +2
 lds r19, @0 +3
 lds r20, @1
 lds r21, @1 +1
 lds r22, @1 +2
 lds r23, @1 +3

 add r19, r23
 adc r18, r22
 adc r17, r21
 adc r16, r20

 sts @2, r16
 sts @2 +1, r17
 sts @2 +2, r18
 sts @2 +3, r19

 .endm
 //----------------------------------------

 //-------- add const double word ----------------
 // addidw A, Kh, Kl, C
 // C = A + Kh:Kl
 // A, C - is a pointers to a word
 // Kh - is a high word number
 // Kl - is a low word number

 .macro addidw

 lds r16, @0
 lds r17, @0 +1
 lds r18, @0 +2
 lds r19, @0 +3
 ldi r20, high(@1)
 ldi r21, low(@1)
 ldi r22, high(@2)
 ldi r23, low(@2)

 add r19, r23
 adc r18, r22
 adc r17, r21
 adc r16, r20

 sts @3, r16
 sts @3 +1, r17
 sts @3 +2, r18
 sts @3 +3, r19

 .endm
 //----------------------------------------

 //-------- mul double word ----------------------
 // muldw A, B, C
 // C = A * B
 // A, B, C - is a pointers to a double word

 .macro muldw

 lds r16, @0
 lds r17, @0 +1
 lds r18, @0 +2
 lds r19, @0 +3
 lds r20, @1
 lds r21, @1 +1
 lds r22, @1 +2
 lds r23, @1 +3
 clr r24
 clr r25
 clr r26
 clr r27
 clr r28

 clt
 tst r16
 brpl muldw_2
 com r16
 com r17
 com r18
 com r19
 ldi r30, 1
 add r19, r30
 clr r30
 adc r18, r30
 adc r17, r30
 adc r16, r30
 set

 muldw_2:
 tst r20
 brpl muldw_lbl
 com r20
 com r21
 com r22
 com r23
 ldi r30, 1
 add r23, r30
 clr r30
 adc r22, r30
 adc r21, r30
 adc r20, r30

 brtc PC+3
 clt
 rjmp PC+2
 set

 muldw_lbl:
 mul r16, r20
 mov r24, r1
 mov r25, r0
 mul r18, r21
 mov r26, r0
 add r25, r1
 adc r16, r28
 mul r17, r22
 add r26, r0
 adc r25, r1
 adc r24, r28
 mul r19, r23
 mov r27, r0
 add r26, r1
 adc r25, r28
 adc r24, r28

 brtc muldw_ret
 tst r24
 brne PC+3
 ser r24
 rjmp PC+3
 neg r24
 dec r24

 tst r25
 brne PC+3
 ser r25
 rjmp PC+3
 neg r25
 dec r25

 tst r26
 brne PC+3
 ser r26
 rjmp PC+3
 neg r26
 dec r26

 tst r27
 brne PC+3
 ser r27
 rjmp PC+3
 neg r27 
 dec r27

 ldi r30, 1
 add r27, r30
 clr r30
 adc r26, r30
 adc r25, r30
 adc r24, r30

 muldw_ret:
 sts @2, r24
 sts @2 +1, r25
 sts @2 +2, r26
 sts @2 +3, r27

 .endm
 //----------------------------------------

 //-------- mul const double word ----------------
 // mulidw A, Kh, Kl, C
 // C = A * K
 // A, C - is a pointers to a double word
 // Kh - is high word number
 // Kl - is low word number

 .macro mulidw

 lds r16, @0
 lds r17, @0 +1
 lds r18, @0 +2
 lds r19, @0 +3
 mov r20, r16
 mov r21, r17
 mov r22, r18
 mov r23, r19
 ldi r24, high(@1)
 ldi r25, low(@1)
 ldi r26, high(@2)
 ldi r27, low(@2)
 ldi r28, 1
 clr r29

 mulidw_lbl:
 sub r27, r28
 sbc r26, r29
 sbc r25, r29
 sbc r24, r29

 add r19, r23
 adc r18, r22
 adc r17, r21
 adc r16, r20
 
 tst r24
 brne mulidw_lbl
 tst r25
 brne mulidw_lbl
 tst r26
 brne mulidw_lbl
 tst r27
 brne mulidw_lbl

 sts @3, r16
 sts @3 +1, r17
 sts @3 +2, r18
 sts @3 +3, r19

 .endm
 //----------------------------------------

 //-------- div double word ----------------
 // divdw A, B, C
 // C = A / B
 // A, B, C - is a pointers to a double word

 .macro divdw

 lds r16, @0
 lds r17, @0 +1
 lds r18, @0 +2
 lds r19, @0 +3
 lds r20, @1
 lds r21, @1 +1
 lds r22, @1 +2
 lds r23, @1 +3
 clr r24
 clr r25
 clr r26
 clr r27
 ldi r28, 1
 clr r29

 clt
 tst r16
 brpl divdw_2
 com r16
 com r17
 com r18
 com r19
 ldi r30, 1
 add r19, r30
 clr r30
 adc r18, r30
 adc r17, r30
 adc r16, r30
 set

 divdw_2:
 tst r20
 brpl divdw_lbl
 com r20
 com r21
 com r22
 com r23
 ldi r30, 1
 add r23, r30
 clr r30
 adc r22, r30
 adc r21, r30
 adc r20, r30

 brtc PC+3
 clt
 rjmp PC+2
 set
 
 divdw_lbl:
 add r27, r28
 adc r26, r29
 adc r25, r29
 adc r24, r29

 sub r19, r23
 sbc r18, r22
 sbc r17, r21
 sbc r16, r20
 brpl divdw_lbl
 
 brtc divdw_ret
 tst r24
 brne PC+3
 ser r24
 rjmp PC+3
 neg r24
 dec r24

 tst r25
 brne PC+3
 ser r25
 rjmp PC+3
 neg r25
 dec r25

 tst r26
 brne PC+3
 ser r26
 rjmp PC+3
 neg r26
 dec r26

 tst r27
 brne PC+3
 ser r27
 rjmp PC+3
 neg r27 
 dec r27

 ldi r30, 1
 add r27, r30
 clr r30
 adc r26, r30
 adc r25, r30
 adc r24, r30

 divdw_ret:
 sts @2, r24
 sts @2 +1, r25
 sts @2 +2, r26
 sts @2 +3, r27

 .endm
 //----------------------------------

 //-------- div const double word ----------------
 // dividw A, Kh, Kl, C
 // C = A / Kh:Kl
 // A, C - is a pointers to a double word
 // Kh - is high word number
 // Kl - is low word number

 .macro dividw

 lds r16, @0
 lds r17, @0 +1
 lds r18, @0 +2
 lds r19, @0 +3
 ldi r20, high(@1)
 ldi r21, low(@1)
 ldi r22, high(@2)
 ldi r23, low(@2)
 clr r24
 clr r25
 clr r26
 clr r27
 ldi r28, 1
 clr r29

 dividw_lbl:
 add r27, r28
 adc r26, r29
 adc r25, r29
 adc r24, r29

 sub r19, r23
 sbc r18, r22
 sbc r17, r21
 sbc r16, r20
 brpl dividw_lbl

 sts @3, r24
 sts @3 +1, r25
 sts @3 +2, r26
 sts @3 +3, r27

 .endm
 //----------------------------------

 //------- sub double word with word -------------
 // subdww A, B, C
 // C = A - B
 // A, B, C - is a pointers to a double word

 .macro subdww

 lds r16, @0
 lds r17, @0 +1
 lds r18, @0 +2
 lds r19, @0 +3
 lds r20, @1
 lds r21, @1 +1

 sub r19, r21
 sbc r18, r20

 sts @2, r16
 sts @2 +1, r17
 sts @2 +2, r18
 sts @2 +3, r19

 .endm
 //----------------------------------------

 //-------- add double word with word ------------
 // adddww A, B, C
 // C = A + B
 // A, B, C - is a pointers to a double word

 .macro adddww

 lds r16, @0
 lds r17, @0 +1
 lds r18, @0 +2
 lds r19, @0 +3
 lds r20, @1
 lds r21, @1 +1
 clr r22

 add r19, r21
 adc r18, r20
 adc r17, r22

 sts @2, r16
 sts @2 +1, r17
 sts @2 +2, r18
 sts @2 +3, r19

 .endm
 //----------------------------------------

 //-------- mul double word with word ------------
 // muldww A, B, C
 // C = A * B
 // A, B, C - is a pointers to a double word

 .macro muldww

 lds r16, @0
 lds r17, @0 +1
 lds r18, @0 +2
 lds r19, @0 +3
 clr r20
 clr r21
 lds r22, @1
 lds r23, @1 +1
 clr r24
 clr r25
 clr r26
 clr r27
 clr r28

 clt
 tst r16
 brpl muldw_2
 com r16
 com r17
 com r18
 com r19
 ldi r30, 1
 add r19, r30
 clr r30
 adc r18, r30
 adc r17, r30
 adc r16, r30
 set

 muldw_2:
 tst r22
 brpl muldw_lbl
 com r22
 com r23
 ldi r30, 1
 add r23, r30
 clr r30
 adc r22, r30

 brtc PC+3
 clt
 rjmp PC+2
 set

 muldw_lbl:
 mul r16, r20
 mov r24, r1
 mov r25, r0
 mul r18, r21
 mov r26, r0
 add r25, r1
 adc r16, r28
 mul r17, r22
 add r26, r0
 adc r25, r1
 adc r24, r28
 mul r19, r23
 mov r27, r0
 add r26, r1
 adc r25, r28
 adc r24, r28

 brtc muldw_ret
 tst r24
 brne PC+3
 ser r24
 rjmp PC+3
 neg r24
 dec r24

 tst r25
 brne PC+3
 ser r25
 rjmp PC+3
 neg r25
 dec r25

 tst r26
 brne PC+3
 ser r26
 rjmp PC+3
 neg r26
 dec r26

 tst r27
 brne PC+3
 ser r27
 rjmp PC+3
 neg r27 
 dec r27

 ldi r30, 1
 add r27, r30
 clr r30
 adc r26, r30
 adc r25, r30
 adc r24, r30

 muldw_ret:
 sts @2, r24
 sts @2 +1, r25
 sts @2 +2, r26
 sts @2 +3, r27

 .endm
 //----------------------------------------

 //-------- div double word with word ------------
 // divdww A, B, C
 // C = A / B
 // A, B, C - is a pointers to a double word

 .macro divdww

 lds r16, @0
 lds r17, @0 +1
 lds r18, @0 +2
 lds r19, @0 +3
 lds r20, @1
 lds r21, @1 +1
 clr r22
 clr r23
 clr r24
 clr r25
 clr r26
 clr r27
 ldi r28, 1
 clr r29

 divdw_lbl:
 add r27, r28
 adc r26, r29
 adc r25, r29
 adc r24, r29

 sub r19, r23
 sbc r18, r22
 sbc r17, r21
 sbc r16, r20
 brpl divdw_lbl

 sts @2, r24
 sts @2 +1, r25
 sts @2 +2, r26
 sts @2 +3, r27

 .endm
 //----------------------------------

 //-------- mul word to double word---------------
 // mulwdw A, B, C
 // C = A * B
 // A, B, C - is a pointers to a word

 .macro mulwdw

 clr r16
 clr r17
 clr r20
 clr r21
 lds r18, @0
 lds r19, @0 +1 
 lds r22, @1
 lds r23, @1 +1
 clr r24

 clt
 tst r18
 brpl muldw_2
 com r18
 com r19
 ldi r30, 1
 add r19, r30
 clr r30
 adc r18, r30
 set

 muldw_2:
 tst r22
 brpl muldw_lbl
 com r22
 com r23
 ldi r30, 1
 add r23, r30
 clr r30
 adc r22, r30

 brtc PC+3
 clt
 rjmp PC+2
 set

 muldw_lbl:
 mul r18, r22
 mov r16, r1
 mov r17, r0
 mul r18, r23
 mov r20, r0
 add r17, r1
 adc r16, r24
 mul r19, r22
 add r20, r0
 adc r17, r1
 adc r16, r24
 mul r19, r23
 mov r21, r0
 add r20, r1
 adc r17, r24
 adc r16, r24

 brtc muldw_ret
 tst r16
 brne PC+3
 ser r16
 rjmp PC+3
 neg r16
 dec r16

 tst r17
 brne PC+3
 ser r17
 rjmp PC+3
 neg r17
 dec r17

 tst r20
 brne PC+3
 ser r20
 rjmp PC+3
 neg r20
 dec r20

 tst r21
 brne PC+3
 ser r21
 rjmp PC+3
 neg r21 
 dec r21

 ldi r30, 1
 add r21, r30
 clr r30
 adc r20, r30
 adc r17, r30
 adc r16, r30

 muldw_ret:
 sts @2, r16
 sts @2 +1, r17
 sts @2 +2, r20
 sts @2 +3, r21

 .endm
 //----------------------------------------

 //-------- mul const word to double word --------
 // mulidw A, K, C
 // C = A * K
 // A, C - is a pointers to a double word
 // Kh - is high word number
 // Kl - is low word number

 .macro muliwdw

 clr r16
 clr r17
 lds r18, @0
 lds r19, @0 +1
 mov r20, r16
 mov r21, r17
 mov r22, r18
 mov r23, r19
 clr r24
 clr r25
 ldi r26, high(@1)
 ldi r27, low(@1)
 ldi r28, 1
 clr r29

 mulidw_lbl:
 sub r27, r28
 sbc r26, r29
 sbc r25, r29
 sbc r24, r29

 add r19, r23
 adc r18, r22
 adc r17, r21
 adc r16, r20
 
 tst r24
 brne mulidw_lbl
 tst r25
 brne mulidw_lbl
 tst r26
 brne mulidw_lbl
 tst r27
 brne mulidw_lbl

 sts @2, r16
 sts @2 +1, r17
 sts @2 +2, r18
 sts @2 +3, r19

 .endm
 //----------------------------------------

 //-------- Shift word left ---------------
 // swl A, K, C
 // C = A << K
 // A, C - is a pointers to a double word
 // K - is const
 .macro swl

 lds r16, @0
 lds r17, @0 +1
 ldi r18, @1

 swl_lbl:
 lsl r17
 brcc PC+4
 lsl r16
 ori r16, 1
 rjmp PC+2
 lsl r16
 dec r18
 brne swl_lbl

 sts @2, r16
 sts @2 +1, r17

 .endm
 //----------------------------------------

 //-------- Shift double word left --------
 // sdwl A, K, C
 // C = A << K
 // A, C - is a pointers to a double word
 // K - is const
 .macro sdwl

 lds r16, @0
 lds r17, @0 +1
 lds r18, @0 +2
 lds r19, @0 +3
 ldi r20, @1

 swl_lbl:
 lsl r19
 brcc PC+4
 lsl r18
 ori r18, 1
 rjmp PC+2
 lsl r18

 brcc PC+4
 lsl r17
 ori r17, 1
 rjmp PC+2
 lsl r17

 brcc PC+4
 lsl r16
 ori r16, 1
 rjmp PC+2
 lsl r16
 dec r18
 brne swl_lbl

 sts @2, r16
 sts @2 +1, r17
 sts @2 +2, r18
 sts @2 +3, r19

 .endm
 //----------------------------------------

 //-------- Shift word left to double -----
 // swldw A, K, C
 // C = A << K
 // A, C - is a pointers to a double word
 // K - is const
 .macro swldw

 clr r16
 clr r17
 lds r18, @0
 lds r19, @0 +1
 ldi r20, @1

 clt
 tst r18
 brpl swl_lbl
 com r18
 com r19
 ldi r21, 1
 add r19, r21
 clr r21
 adc r18, r21
 set

 swl_lbl:
 lsl r19
 brcc PC+4
 lsl r18
 ori r18, 0x01
 rjmp PC+2
 lsl r18

 brcc PC+4
 lsl r17
 ori r17, 0x01
 rjmp PC+2
 lsl r17

 brcc PC+4
 lsl r16
 ori r16, 0x01
 rjmp PC+2
 lsl r16

 dec r20
 brne swl_lbl

 brtc swl_ret
 tst r16
 brne PC+3
 ser r16
 rjmp PC+3
 neg r16
 dec r16

 tst r17
 brne PC+3
 ser r17
 rjmp PC+3
 neg r17
 dec r17

 tst r18
 brne PC+3
 ser r18
 rjmp PC+3
 neg r18
 dec r18

 tst r19
 brne PC+3
 ser r19
 rjmp PC+3
 neg r19 
 dec r19

 ldi r21, 1
 add r19, r21
 clr r21
 adc r18, r21
 adc r17, r21
 adc r16, r21

 swl_ret:
 sts @2, r16
 sts @2 +1, r17
 sts @2 +2, r18
 sts @2 +3, r19

 .endm
 //----------------------------------------

 //-------- Shift word right ---------------
 // swl A, K, C
 // C = A >> K
 // A, C - is a pointers to a double word
 // K - is const
 .macro swr

 lds r16, @0
 lds r17, @0 +1
 ldi r18, @1

 clt
 dec r18
 tst r16
 brpl swr_lbl
 com r16
 com r17
 ldi r19, 1
 add r17, r19
 clr r19
 adc r16, r19
 set

 swr_lbl:
 lsr r16
 brcc PC+4
 lsr r17
 ori r17, 0x80
 rjmp PC+2
 lsr r17
 dec r18
 brne swr_lbl

 tst r16
 brne PC+3
 cpi r17, 1
 breq swr_ret

 lsr r16
 brcc PC+4
 lsr r17
 ori r17, 0x80
 rjmp PC+2
 lsr r17

 swr_ret:
 brtc swr_retret
 tst r16
 brne PC+3
 ser r16
 rjmp PC+3
 neg r16
 dec r16

 tst r17
 brne PC+3
 ser r17
 rjmp PC+3
 neg r17
 dec r17

 ldi r21, 1
 add r17, r21
 clr r21
 adc r16, r21

 swr_retret:
 sts @2, r16
 sts @2 +1, r17

 .endm
 //----------------------------------------

 //-------- Shift double word right --------
 // sdwr A, K, C
 // C = A >> K
 // A, C - is a pointers to a double word
 // K - is const
 .macro sdwr

 lds r16, @0
 lds r17, @0 +1
 lds r18, @0 +2
 lds r19, @0 +3
 ldi r20, @1

 clt
 dec r20
 tst r16
 brpl sdwr_lbl
 com r16
 com r17
 com r18
 com r19
 ldi r21, 1
 add r19, r21
 clr r21
 adc r18, r21
 adc r17, r21
 adc r16, r21
 set

 sdwr_lbl:
 lsr r16
 brcc PC+4
 lsr r17
 ori r17, 0x80
 rjmp PC+2
 lsr r17

 brcc PC+4
 lsr r18
 ori r18, 0x80
 rjmp PC+2
 lsr r18

 brcc PC+4
 lsr r19
 ori r19, 0x80
 rjmp PC+2
 lsr r19
 dec r20
 brne sdwr_lbl

 tst r16
 brne sdwr_sh
 tst r17
 brne sdwr_sh
 tst r18
 brne sdwr_sh
 cpi r19, 1
 breq sdwr_s

 sdwr_sh:
 lsr r16
 brcc PC+4
 lsr r17
 ori r17, 0x80
 rjmp PC+2
 lsr r17

 brcc PC+4
 lsr r18
 ori r18, 0x80
 rjmp PC+2
 lsr r18

 brcc PC+4
 lsr r19
 ori r19, 0x80
 rjmp PC+2
 lsr r19

 sdwr_s:
 brtc sdwr_ret
 tst r16
 brne PC+3
 ser r16
 rjmp PC+3
 neg r16
 dec r16

 tst r17
 brne PC+3
 ser r17
 rjmp PC+3
 neg r17
 dec r17

 tst r18
 brne PC+3
 ser r18
 rjmp PC+3
 neg r18
 dec r18

 tst r19
 brne PC+3
 ser r19
 rjmp PC+3
 neg r19 
 dec r19

 ldi r21, 1
 add r19, r21
 clr r21
 adc r18, r21
 adc r17, r21
 adc r16, r21

 sdwr_ret:
 sts @2, r16
 sts @2 +1, r17
 sts @2 +2, r18
 sts @2 +3, r19

 .endm
 //----------------------------------------

 //-------- push all registers ------------
 .macro pusha
 push r16
 push r17
 push r18
 push r19
 push r20
 push r21
 push r22
 push r23
 push r24
 push r25
 push r26
 push r27
 push r28
 push r29
 push r30
 push r31
 .endm
 //----------------------------------------

 //-------- pop all registers -------------
 .macro popa
 push r31
 push r30
 push r29
 push r28
 push r27
 push r26
 push r25
 push r24
 push r23
 push r22
 push r21
 push r20
 push r19
 push r18
 push r17
 push r16
 .endm
 //----------------------------------------

 /*.macro tmul32

 lds r16, @0
 lds r17, @0 +1
 lds r18, @0 +2
 lds r19, @0 +3
 lds r20, @1
 lds r21, @1 +1
 lds r22, @1 +2
 lds r23, @1 +3

 call mul32_p
 
 sts @2, r24
 sts @2 +1, r25
 sts @2 +2, r26
 sts @2 +3, r27

 .endm
 //----------------------------------------

 //----------------------------------------

 .macro mul16

 clr r16
 clr r17
 lds r18, @0
 lds r19, @0 +1
 clr r20
 clr r21
 lds r22, @1
 lds r23, @1 +1

 call mul32_p
 
 sts @2, r24
 sts @2 +1, r25
 sts @2 +2, r26
 sts @2 +3, r27

 .endm
 //----------------------------------------

 //----------------------------------------

 .macro muli32

 lds r16, @0
 lds r17, @0 +1
 lds r18, @0 +2
 lds r19, @0 +3
 ldi r20, high(high(@1))
 ldi r21, high(low(@1))
 ldi r22, low(high(@1))
 ldi r23, low(low(@1))

 call mul32_p
 
 sts @2, r24
 sts @2 +1, r25
 sts @2 +2, r26
 sts @2 +3, r27

 .endm
 //----------------------------------------

 //----------------------------------------

 .macro muli16

 clr r16
 clr r17
 lds r18, @0
 lds r19, @0 +1
 clr r20
 clr r21
 ldi r22, high(@1)
 ldi r23, low(@1)

 call mul32_p
 
 sts @2, r24
 sts @2 +1, r25
 sts @2 +2, r26
 sts @2 +3, r27

 .endm
 //----------------------------------------

 //----------------------------------------
 //========================================

 //========= PROCEDURES ===================
  ;***********************************************************
;*
;* Mul32 == 32x32 Bit Unsigned Multiplication
;*
;* mp32uL::mp32uH  x  mc32uL::mc32uH  =  m32uL::m32uH
;*   multiplier        multiplicand        result
;*  r20r21r22r23   x   r16r17r18r19   =  r20r21r22r23r24r25r26r27
;*
;***********************************************************
mul32_p:

.def    mc32u0    =r16    ; multiplicand byte 0 (LSB)
.def    mc32u1    =r17    ; multiplicand byte 1
.def    mc32u2    =r18    ; multiplicand byte 2
.def    mc32u3    =r19    ; multiplicand byte 3 (MSB)
.def    mp32u0    =r20    ; multiplier byte 0 (LSB)
.def    mp32u1    =r21    ; multiplier byte 1
.def    mp32u2    =r22    ; multiplier byte 2
.def    mp32u3    =r23    ; multiplier byte 3 (MSB)
.def    m32u0    =r20    ; result byte 0 (LSB)
.def    m32u1    =r21    ; result byte 1
.def    m32u2    =r22    ; result byte 2
.def    m32u3    =r23    ; result byte 3
.def    m32u4    =r24    ; result byte 4
.def    m32u5    =r25    ; result byte 5
.def    m32u6    =r26    ; result byte 6
.def    m32u7    =r27    ; result byte 7 (MSB)
.def    mcnt32u =r28     ; loop counter
       
Mul32:    clr    m32u7        ;clear 4 highest bytes of result
    clr    m32u6        ;
    clr    m32u5        ;
    sub    m32u4,m32u4      ;and carry
    ldi    mcnt32u,33            ;init loop counter
m32u_loop:   
    ror    m32u3                   ;rotate result and multiplier
    ror    m32u2       
    ror    m32u1       
    ror    m32u0       
    dec    mcnt32u               ;decrement loop counter
    breq    Return32u           ;if counter zero return
    brcc    m32u_skip          ;if bit 0 of multiplier set
    add    m32u4,mc32u0    ;add multiplicand to result
    adc    m32u5,mc32u1   
    adc    m32u6,mc32u2  
    adc    m32u7,mc32u3   
m32u_skip:    ror    m32u7 ;shift right result byte 7
    ror    m32u6                  ;rotate right result
    ror    m32u5       
    ror    m32u4       
rjmp    m32u_loop   
Return32u:   
ret
 //========================================*/
