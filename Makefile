CFLAGS=-Wall -g -Os -I"include" -mmcu=$(MCU) -o
DFLAGS=-j .text -j .data -O ihex 

MCU=atmega328
PORT=COM4
PROGRAMMER=arduino

clear:
	rm bin/*.o
	rm bin/main.hex

build:
	E:\avr-devel\avr8-gnu-toolchain-win32_x86\bin\avr-gcc $(CFLAGS) "bin/main.o" main.c include/*.c
	E:\avr-devel\avr8-gnu-toolchain-win32_x86\bin\avr-objcopy $(DFLAGS) "bin/main.o" "bin/main.hex"
	E:\avr-devel\avr8-gnu-toolchain-win32_x86\bin\avr-size --mcu=$(MCU) -C "bin/main.o"

flash:
	C:\Program Files (x86)\AVRDUDESS\avrdude -p m328p -P $(PORT) -v -v -c $(PROGRAMMER) -U flash:w:bin/main.hex:i -b115200 -B 0.5

arduinoFlash:
	E:\avr-devel\avr8-gnu-toolchain-win32_x86\bin\avr-gcc $(CFLAGS) "bin/main.o" main.c include/*.c
	E:\avr-devel\avr8-gnu-toolchain-win32_x86\bin\avr-objcopy $(DFLAGS) "bin/main.o" "bin/main.hex"
	E:\avr-devel\avr8-gnu-toolchain-win32_x86\bin\avr-size --mcu=$(MCU) -C "bin/main.o"

	
	C:\Program Files (x86)\AVRDUDESS\avrdude -c $(PROGRAMMER) -p m328p -P $(PORT) -v -v -U flash:w:bin\optiboot_atmega328.hex
	C:\Program Files (x86)\AVRDUDESS\avrdude -c $(PROGRAMMER) -p m328p -P $(PORT) -v -v -U lock:w:0x0f:m -F
	C:\Program Files (x86)\AVRDUDESS\avrdude -c $(PROGRAMMER) -p m328p -P $(PORT) -v -v -D -U flash:w:bin/main.hex:i


monitor:
	putty -load M328P