all: apmath.o angel.o
	cc *.o

apmath.o: apmath.c
	cc -c apmath.c
angel.o: angel.c
	cc -c angel.c

clean:
	rm -f *.out *.o
