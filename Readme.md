Upload to bare Atmega328p :
make -j8 && reset_atmega328.sh && avrdude  -pm328p -c usbasp -P usb -b38400 -D -Uflash:w:$(ls *.hex):i

Upload to Arduino pro mini :
avrdude  -pm328p -carduino -b57600   -carduino  -D -Uflash:w:$(ls *.hex):i -P /dev/ttyUSB0

Upload to Atmega2560 :
make -j6 && avrdude -p m2560 -c wiring -P /dev/ttyACM0 -F -U flash:w:$(ls *.hex)


Clear ATMEGA2560 :
/usr/share/arduino/hardware/tools/avrdude -C/usr/share/arduino/hardware/tools/avrdude.conf -v -v -v -v -patmega2560 -cusbasp -Pusb -Uflash:w:/usr/share/arduino/hardware/arduino/bootloaders/stk500v2/stk500boot_v2_mega2560.hex:i -Ulock:w:0x0F:m
reprogram with blinker :
/usr/share/arduino/hardware/tools/avrdude -C/usr/share/arduino/hardware/tools/avrdude.conf -v -v -v -v -patmega2560 -cwiring -P/dev/ttyACM0 -b115200 -D -Uflash:w:/tmp/build5053554090854919602.tmp/BlinkWithoutDelay.cpp.hex:i
