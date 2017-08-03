EXE = UCode
CFLAGS = -lpthread

$(EXE):main.c config.h
	gcc -o $@  $< $(CFLAGS)

clean:
	@rm *.o $(EXE)
