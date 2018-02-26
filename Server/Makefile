ifneq ($(KERNELRELEASE),)

obj-y := src/

obj-m := ResourceMonitorServer.o

ResourceMonitorServer-objs := main.o src/built-in.o
EXTRA_LDFLAGS := --start-group
KBUILD_CFLAGS +=-Wno-declaration-after-statement -Wno-strict-prototypes -msoft-float -lsoft-fp -mno-sse
EXTRA_CFLAGS := -DMMAN -I$(src)/include

else

KSRC = /lib/modules/$(shell uname -r)/build

PWD = $(shell pwd)

default:
	$(MAKE) -C $(KSRC) M=$(PWD)

install:
	sudo cp ResourceMonitorServer.conf /etc
	sudo insmod ResourceMonitorServer.ko

monitor:
	sudo tail -f /var/log/syslog

uninstall:
	sudo rmmod ResourceMonitorServer

clean:
	$(MAKE) -C $(KSRC) M=$(PWD) clean

endif
