CC = gcc
LIB = -lpthread
EXE0 = Example_7.3.exe
EXE1 = Example_7.7.exe

%.exe : %.o 
	$(CC) $(LIB) -o $@ $^

%.o : %.c
	$(CC) $(LIB) -c -o $@ $^

all: $(EXE0) $(EXE1)

clean:
	rm -f *.o
