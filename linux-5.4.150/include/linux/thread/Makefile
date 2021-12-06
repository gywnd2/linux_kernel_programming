# obj-m := hello_module.o

# KERNEL_DIR :=/lib/modules/$(shell uname -r)/build
# PWD :=$(shell pwd)

# default:
# 	$(MAKE) -C $(KERNEL_DIR) SUBDIRS=$(PWD)modules
# clean:
# 	$(MAKE) -C $(KERNEL_DIR) SUBDIRS=$(PWD)clean

obj-m +=hello_module.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

default:
		$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
		rm *.mod.*
		rm *.ko
		rm *.o