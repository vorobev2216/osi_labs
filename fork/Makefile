# Флаги компилятора
CFLAGS = -Wall -g

all: myfork

myfork: ./main.c
	gcc $(CFLAGS) -o myfork ./main.c

clean:
	rm -f myfork

rebuild: clean all
