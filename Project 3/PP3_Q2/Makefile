CC = gcc
LIB = -fopenmp
EXE0 = Gauss-Elim.exe

%.exe : %.o 
	$(CC) $(LIB) -o $@ $^

%.o : %.c
	$(CC) $(LIB) -c -o $@ $^

all: $(EXE0)

clean:
	rm -f *.o
