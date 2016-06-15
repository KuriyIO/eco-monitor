/*
 * I2C.inc
 *
 *  Created: 09.06.2016 16:02:41
 *   Author: Hogs
 */ 


 //=========== MACRO =====================================

 //------- i2c init --------------------------------
 .macro I2C_Init
 ldi temp,60
 out TWBR,temp		//set SCL frequency as 100KHz
 .endm

 //-------------------------------------------------
 //------- Write single byte -----------------------

 // example: i2c_write_sbyte SLA, pointer, data
 .macro i2c_write_sbyte
 push r16
 push r17
 push r19

  ldi  r18, 11
    ldi  r19, 99
L1: dec  r19
    brne L1
    dec  r18
    brne L1

 mov r18, @2
 clr r19
 ldi byte, @0
 ldi temp, @1 

    

 call i2c_write

 


 pop r19
 pop r17
 pop r16
 .endm
 //-------------------------------------------------

 //------- Write multiple bytes --------------------
 .macro i2c_write_mbyte
 push r16
 push r17
 push r19
 push XH
 push XL

  ldi  r18, 11
    ldi  r19, 99
L1: dec  r19
    brne L1
    dec  r18
    brne L1

 ldi byte, @0
 ldi temp, @1
 ldi XH, high(@2)
 ldi XL, low (@2)
 ldi r19, @3
 ld X+, r18
 dec r19


 call i2c_write

    


 pop XL
 pop XH
 pop r19
 pop r17
 pop r16
 .endm
 //-------------------------------------------------

 //------- Read single byte ------------------------
 .macro i2c_read_sbyte
 push r16
 push r17
 push r19

    ldi  r18, 11
    ldi  r19, 99
L1: dec  r19
    brne L1
    dec  r18
    brne L1

 clr r19
 ldi byte, @0
 ldi temp, @1

 call i2c_read

 


 pop r19
 pop r17
 pop r16
 .endm
 //-------------------------------------------------

 //------- Read multiple bytes ---------------------
 .macro i2c_read_mbyte
 push r16
 push r17
 push r18
 push r19
 push XH
 push XL

    ldi  r18, 11
    ldi  r19, 99
L1: dec  r19
    brne L1
    dec  r18
    brne L1

 ldi byte, @0
 ldi temp, @1
 ldi XH, high(@2)
 ldi XL, low (@2)
 ldi r19, @3

 call i2c_read




 pop XL
 pop XH
 pop r19
 pop r18
 pop r17
 pop r16
 .endm
 //-------------------------------------------------

 //=========== PROCEDURES ================================

 //------- Write byte proc -------------------------

 // input:
 // SLA(device id) - r16 (byte)
 // data pointer - r17 (temp)
 // data (if single byte) - r18 (data)
 // count of bytes(if multiple) - r19
 // buffer pointer(if multiple) - X
 //
 // no output

 i2c_write:

 push temp
 /* Send Start */
 ldi temp, ((1<<TWSTA)|(1<<TWINT)|(1<<TWEN))
 out TWCR,temp

 in temp, TWCR
 sbrs temp, TWINT
 rjmp PC-2

 /* Send device id + W */
 in temp, TWSR
 andi temp, ~((1<<TWPS1) | (1<<TWPS0))
 cpi temp,0x08
 brne i2c_write_err		//restart on error

 cbr byte, 1			//set W bit
 out TWDR,byte			//load to TWDR SLA+W(device id + write)
 ldi temp, ((1<<TWINT)|(1<<TWEN))	//send SLA+W(device id + write)
 out TWCR,temp

 in temp, TWCR
 sbrs temp, TWINT
 rjmp PC-2

 /* Write pointer */
 in temp, TWSR
 andi temp, ~((1<<TWPS1) | (1<<TWPS0))
 cpi temp,0x18
 brne i2c_write_err		//restart on error

 pop temp
 out TWDR,temp
 ldi temp, ((1<<TWINT)|(1<<TWEN))
 out TWCR,temp

 in temp, TWCR
 sbrs temp, TWINT
 rjmp PC-2

 /* Write data byte(s) */
 i2c_write_b:
 in temp, TWSR
 andi temp, ~((1<<TWPS1) | (1<<TWPS0))
 cpi temp,0x28
 brne i2c_write_err		//restart on error

 out TWDR, r18
 ldi temp, ((1<<TWINT)|(1<<TWEN))
 out TWCR,temp

 in temp, TWCR
 sbrs temp, TWINT
 rjmp PC-2

 tst r19
 breq i2c_write_stop

 ld r18, X+
 dec r19
 rjmp i2c_write_b

 /* Write stop */
 i2c_write_stop:
 ldi temp, ((1<<TWSTO)|(1<<TWINT)|(1<<TWEN))	//generating stop
 out TWCR,temp

 ret

 /* Restart On Error */
 i2c_write_err:
 ldi temp, ((1<<TWSTO)|(1<<TWINT)|(1<<TWEN))	//generating stop
 out TWCR,temp

 rjmp i2c_write+1
 //---------------------------------------------------

 //--------- Read byte proc --------------------------
 // input:
 // SLA(device id) - r16 (byte)
 // data pointer(if single) - r17 (temp)
 // count of bytes(if multiple) - r19
 // buffer pointer(if multiple) - X
 //
 // output:
 // data(if single) - r18 (data)

 i2c_read:

 push temp

 /* Send Start */
 ldi temp, ((1<<TWSTA)|(1<<TWINT)|(1<<TWEN))
 out TWCR,temp

 in temp, TWCR
 sbrs temp, TWINT
 rjmp PC-2

 /* Send SLA+W */
 in temp, TWSR
 andi temp, ~((1<<TWPS1) | (1<<TWPS0))
 cpi temp,0x08
 breq PC+2
 rjmp i2c_read_err		//restart on error

 cbr byte, 1			//set W bit
 out TWDR,byte			//load to TWDR SLA+W(device id + write)
 ldi temp, ((1<<TWINT)|(1<<TWEN))	//send SLA+W(device id + write)
 out TWCR,temp

 in temp, TWCR
 sbrs temp, TWINT
 rjmp PC-2 

 /* Write pointer */
 in temp, TWSR
 andi temp, ~((1<<TWPS1) | (1<<TWPS0))
 cpi temp,0x18
 brne i2c_read_err		//restart on error

 pop temp
 out TWDR,temp
 ldi temp, ((1<<TWINT)|(1<<TWEN))
 out TWCR,temp

 in temp, TWCR
 sbrs temp, TWINT
 rjmp PC-2

 /* Write restart */
 in temp, TWSR
 andi temp, ~((1<<TWPS1) | (1<<TWPS0))
 cpi temp,0x28
 brne i2c_read_err		//restart on error

 ldi temp, ((1<<TWSTA)|(1<<TWINT)|(1<<TWEN))	//generating start
 out TWCR,temp

 in temp, TWCR
 sbrs temp, TWINT
 rjmp PC-2

 /* Write SLA+R */
 in temp, TWSR
 andi temp, ~((1<<TWPS1) | (1<<TWPS0))
 cpi temp,0x10
 brne i2c_read_err		//restart on error

 sbr byte, 1			//set R bit
 out TWDR,byte			//load to TWDR SLA+R(device id + read)
 ldi temp, ((1<<TWINT)|(1<<TWEN))
 out TWCR,temp

 in temp, TWCR
 sbrs temp, TWINT
 rjmp PC-2

 /* Read data byte(s) */
 ldi byte, 0x40 
 i2c_read_b:
 in temp, TWSR
 andi temp, ~((1<<TWPS1) | (1<<TWPS0))
 cp temp, byte
 brne i2c_read_err		//restart on error

 cpi r19,2
 brsh PC+3
 ldi temp, ((1<<TWINT)|(1<<TWEN))
 rjmp PC+2
 ldi temp, ((1<<TWEA)|(1<<TWINT)|(1<<TWEN))

 out TWCR,temp 

 in temp, TWCR
 sbrs temp, TWINT
 rjmp PC-2

 in r18,TWDR			//read data
 clr temp
 out TWDR,temp			//clear TWDR

 tst r19
 breq i2c_read_stop

 st X+, r18
 ldi byte, 0x50 
 dec r19
 rjmp i2c_read_b

 /* Send stop */
 i2c_read_stop:
 ldi temp, ((1<<TWSTO)|(1<<TWINT)|(1<<TWEN))	//generating stop
 out TWCR,temp

 ret

 /* Restart On Error */
 i2c_read_err:
 ldi temp, ((1<<TWSTO)|(1<<TWINT)|(1<<TWEN))	//generating stop
 out TWCR,temp

 rjmp i2c_read+1
 //---------------------------------------------------
 //=======================================================