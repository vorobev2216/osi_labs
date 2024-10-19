# Флаги компилятора
CFLAGS = -Wall -g

all: myls

myls: ./main.c
	gcc $(CFLAGS) -o myls ./main.c

clean:
	rm -f myls

rebuild: clean all
