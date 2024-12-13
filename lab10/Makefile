.PHONY: all cond clean
all: cond
cond: condd.c
	gcc condd.c -lpthread -o cond

clean:
	rm -f *.o cond