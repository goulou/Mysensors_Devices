PROJECT_DIR=$(dirname $0)
BOOTLOADER=${PROJECT_DIR}/Arduino/hardware/arduino/avr/bootloaders/optiboot/optiboot_atmega328_384_8.hex

#set fuses
avrdude  -c usbasp -p atmega328p -P usb -b 115200 -u -U lfuse:w:0xe2:m -U hfuse:w:0xd9:m -U efuse:w:0xff:m


avrdude  -c usbasp -p atmega328p -P usb -b 38400 -e -U flash:w:${BOOTLOADER} -D -U lock:w:0x2f:m
