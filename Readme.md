Upload to bare Atmega328p :
make -j8 && reset_atmega328.sh && avrdude  -pm328p -c usbasp -P usb -b38400 -D -Uflash:w:$(ls *.hex):i

Upload to Arduino pro mini :
avrdude  -pm328p -carduino -b57600   -carduino  -D -Uflash:w:$(ls *.hex):i -P /dev/ttyUSB0

Upload to Atmega2560 :
make && avrdude -p m2560 -c stk500v2 -P /dev/ttyACM0 -b 115200 -F -U flash:w:$(ls *.hex)
