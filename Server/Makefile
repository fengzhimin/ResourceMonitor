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
	sudo mkdir -p /var/log/ResourceMonitor/Server
	sudo mkdir -p /etc/ResourceMonitor/Server
	sudo cp ResourceMonitorServer.conf /etc/ResourceMonitor/Server
	sudo insmod ResourceMonitorServer.ko

monitor:
	sudo tail -f /var/log/syslog

uninstall:
	sudo rm -fr /var/log/ResourceMonitor/Server
	sudo rm -fr /etc/ResourceMonitor/Server
	sudo rmmod ResourceMonitorServer

reinstall:
	sudo make uninstall
	sudo make install

clean:
	$(MAKE) -C $(KSRC) M=$(PWD) clean

endif
