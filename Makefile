ifneq ($(KERNELRELEASE),)

obj-y := src/

obj-m := KCode.o

KCode-objs := main.o src/built-in.o
EXTRA_LDFLAGS := --start-group
KBUILD_CFLAGS +=-Wno-declaration-after-statement -Wno-strict-prototypes
EXTRA_CFLAGS := -DMMAN -I$(src)/include

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
