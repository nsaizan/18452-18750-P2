obj-m += ipoa_mod.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules

clean:
	sudo ifconfig IPOA down
	sudo rmmod ipoa_mod
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean

test:
	sudo dmesg -C
	sudo insmod ipoa_mod.ko
	sudo ifconfig IPOA up
