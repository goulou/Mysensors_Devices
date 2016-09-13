if [ -z "$1" ];
then
	PROJECT_DIR=$(dirname $0)
	BOOTLOADER=${PROJECT_DIR}/Arduino/hardware/arduino/avr/bootloaders/optiboot/optiboot_atmega328_384_8.hex
else
	BOOTLOADER=$1
fi

#set fuses
avrdude  -c usbasp -p atmega328p -P usb -b 115200 -u -U lfuse:w:0x62:m -U hfuse:w:0xd9:m -V -U efuse:w:0xff:m || exit -1


avrdude  -c usbasp -p atmega328p -P usb -b 38400 -e -U flash:w:${BOOTLOADER} -D -U lock:w:0x2f:m

