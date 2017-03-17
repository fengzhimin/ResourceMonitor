ifneq ($(KERNELRELEASE),)

obj-y := src/

obj-m := KCode.o

KCode-objs := main.o src/built-in.o
EXTRA_LDFLAGS := --start-group
KBUILD_CFLAGS := -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration

else

KSRC = /lib/modules/$(shell uname -r)/build

PWD = $(shell pwd)

default:
	$(MAKE) -C $(KSRC) M=$(PWD)

install:
	sudo insmod KCode.ko

uninstall:
	sudo rmmod KCode

clean:
	$(MAKE) -C $(KSRC) M=$(PWD) clean

endif
