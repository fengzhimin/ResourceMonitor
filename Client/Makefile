EXE = ResourceMonitor
CFLAGS = -lpthread
obj = main.o strOper.o fileOper.o confOper.o

$(EXE):$(obj)
	gcc -o $@  $^ $(CFLAGS)

install:
	sudo cp $(EXE) /usr/bin

clean:
	@rm *.o $(EXE)
