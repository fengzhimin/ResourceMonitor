#设置编译器
CC=gcc
SUBDIRS=src
#记住当前工程的根目录路径
ROOT_DIR=$(shell pwd)
#最终bin文件的名字，可以更改为自己需要的
BIN=ResourceMonitor
#目标文件所在的目录
OBJS_DIR=release/obj
#bin文件所在的目录
BIN_DIR=release/bin
#获取当前目录下的c文件集，放在变量CUR_SOURCE中
CUR_SOURCE=${wildcard *.c}
#将对应的c文件名转为o文件后放在下面的CUR_OBJS变量中
CUR_OBJS=${patsubst %.c, %.o, $(CUR_SOURCE)}
CFLAGS=	-I $(ROOT_DIR)/include
#将以下变量导出到子shell中，本次相当于导出到子目录下的makefile中
export CC BIN OBJS_DIR BIN_DIR ROOT_DIR CFLAGS
#注意这里的顺序，需要先执行SUBDIRS最后才能是DEBUG
all:$(SUBDIRS) $(CUR_OBJS) RELEASE
#递归执行子目录下的makefile文件，这是递归执行的关键
$(SUBDIRS):ECHO
	make -C $@

#直接去release目录下执行makefile文件
RELEASE:ECHO
	make -C release

ECHO:
	@echo $(SUBDIRS)

#将c文件编译为o文件，并放在指定放置目标文件的目录中即OBJS_DIR
$(CUR_OBJS):%.o:%.c
	$(CC) -c $^ $(CFLAGS) -o $(ROOT_DIR)/$(OBJS_DIR)/$@

install:
	sudo mkdir -p /var/log/ResourceMonitor/Client
	sudo cp ResourceMonitorClient.conf /etc
	sudo cp $(ROOT_DIR)/$(BIN_DIR)/$(BIN) /usr/bin

uninstall:
	sudo rm -fr /var/log/ResourceMonitor/Client
	sudo rm /etc/ResourceMonitorClient.conf
	sudo rm /usr/bin/$(BIN)

clean:
	@rm $(OBJS_DIR)/*.o
	@rm -rf $(BIN_DIR)/*
