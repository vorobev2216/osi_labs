# Флаги компилятора
CFLAGS = -Wall -g

all: mychmod

mychmod: ./main.c
	gcc $(CFLAGS) -o mychmod ./main.c

clean:
	rm -f mychmod

rebuild: clean all
