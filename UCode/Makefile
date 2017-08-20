EXE = UCode
CFLAGS = -lpthread
obj = main.o strOper.o fileOper.o confOper.o

$(EXE):$(obj)
	gcc -o $@  $^ $(CFLAGS)

clean:
	@rm *.o $(EXE)
