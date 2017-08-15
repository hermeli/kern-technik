obj-m += fastgpio.o

all:
	make -C /lib/modules/$(shell uname -r)/linux-source M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/linux-source M=$(PWD) clean
