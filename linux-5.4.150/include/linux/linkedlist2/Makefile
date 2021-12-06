obj-m +=c.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

default:s
		$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
		rm *.mod.*
		rm *.ko
		rm *.o