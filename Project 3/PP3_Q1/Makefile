CC = gcc
LIB = -fopenmp
EXE0 = Example_7.14.exe

%.exe : %.o 
	$(CC) $(LIB) -o $@ $^

%.o : %.c
	$(CC) $(LIB) -c -o $@ $^

all: $(EXE0)

clean:
	rm -f *.o
