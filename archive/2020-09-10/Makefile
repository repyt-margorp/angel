APMATH =	apmath_memory.o apmath_indecrement.o	\
		apmath_addition.o apmath_subtraction.o apmath.o
FLAG =	-O3

all: $(APMATH) angel.o
	cc *.o

apmath_memory.o: apmath_memory.c
	cc $(FLAG) -c apmath_memory.c
apmath_indecrement.o: apmath_indecrement.c
	cc $(FLAG) -c apmath_indecrement.c
apmath_addition.o: apmath_addition.c
	cc $(FLAG) -c apmath_addition.c
apmath_subtraction.o: apmath_subtraction.c
	cc $(FLAG) -c apmath_subtraction.c

apmath.o: apmath.c
	cc -O3 -c apmath.c
angel.o: angel.c
	cc -O3 -c angel.c

clean:
	rm -f *.out *.o
	rm -rf html latex
