/*
 * Temp.inc
 *
 *  Created: 31.05.2016 1:35:52
 *   Author: Hogs
 */

.equ Wire_DDR = DDRA // Регистр порта Датчика
.equ Wire_PORT = PORTA // Регистр порта Датчика
.equ Wire_PIN = PINA // Регистр порта Датчика
.equ Wire_BIT = 0 // Бит порта на котором датчик 


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
 sbi Wire_DDR, Wire_BIT			// Ногу на выход
 cbi Wire_PORT, Wire_BIT			// Опрокидываем вывод на землю
 Wire_delay2 10, 249				// Задержка 480 мкс, для сброса
 cbi Wire_DDR, Wire_BIT			// Ногу на вход
 Wire_delay2 2, 115				// Ждем тайм слот 70 мкс
 sbis Wire_PIN, Wire_BIT		// Пропускаем следующую строку, если бит порта в 1
 ldi temp, 1					// И установим сигнальный регистр в 1
 sbic Wire_PIN, Wire_BIT		// Пропускаем следующую строку, если бит порта в 0
 clr temp						// И установим сигнальный регистр в 0
 Wire_delay2 9, 132				// Ждем тайм слот 410 мкс
 ret
 //---------------------------------------

 //--------- Set 12bit mode --------------
 Wire_Init:  
 Wire_write 0xCC	// Пропускаем уникальный номер датчика
 Wire_write 0x4E	// Говорим что сейчас будем писать в RAM регистры датчика
 Wire_write 0xFF	// 0xFF записываем в первые 2 регистра, это регистры температуры, он нам не 
 Wire_write 0xFF	// нужен, поэтому их оставляем в стандартном состоянии
 Wire_write 0x7F	// А вот тут говорим что точность 12 бит - 7F
 ret
 //---------------------------------------

 //--------- Start measuring -------------
 Wire_Measure:
 Wire_write 0xCC	// Пропускаем уникальный номер датчика
 Wire_write 0x44	// Говорим что надо бы сконвертировать температуру
 ret
 //---------------------------------------

 //--------- Read Temperature ------------
 Wire_ReadTemp:
 Wire_write 0xCC		// Пошлем команду 0xCC, это пропустить уникальный номер датчика
 Wire_write 0xBE		// Говорим датчику, что мы сейчас будем читать
 rcall Wire_Read_proc	// А тут уже начинаем читать, прочитали первый
 sts Wire_temp, byte	// И запулили его в память, по метке Trm
 rcall Wire_Read_proc	// Читаем второй
 sts Wire_temp+1, byte	// И запулили его в память, по метке Trm+1
 ret
 //---------------------------------------

 //---------- Read byte ------------------
 Wire_Read_proc:
 ldi temp, 8
 clr byte
 wire_read_bit:
 sbi Wire_DDR, Wire_BIT		// Вывод на выход
 cbi Wire_PORT, Wire_BIT	// Опрокидываем вывод на землю
 Wire_delay1 32				// Ждем 6 микросекунд
 cbi Wire_DDR, Wire_BIT		// Вывод на вход
 Wire_delay1 48 			// Ждем 9 микросекунд
 sbis Wire_PIN, Wire_BIT
 clc						// Очищаем бит C = 0
 sbic Wire_PIN, Wire_BIT
 sec						// Очищаем бит C = 1
 ror byte					// Производим циклический сдвиг вправо через С
 Wire_delay2 2, 35			// Ждем 55 микросекунд
 dec temp
 brne wire_read_bit
 ret
 //---------------------------------------

 //---------- Write byte -----------------
 Wire_Write_proc:
 ldi temp, 8
 wire_write_bit: 
 sbi Wire_DDR, Wire_BIT			// Вывод на выход
 cbi Wire_PORT, Wire_BIT		// Опрокидываем вывод на землю
 sbrc byte, 0					// Проверим, в r16 бит 0 очищен или установлен 
 rjmp wire_write_1				// Если установлен перейдем по этой метке

 Wire_delay2 2, 62				// Ждем 60 микросекунд
 cbi Wire_DDR, Wire_BIT			// Вывод на вход
 Wire_delay1 53					// Ждем 10 микросекунд
 rjmp wire_next_bit

 wire_write_1:
 Wire_delay1 32					// Ждем 6 микросекунд
 cbi Wire_DDR, Wire_BIT			// Вывод на вход
 Wire_delay2 2, 83					// Ждем 64 микросекунд

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