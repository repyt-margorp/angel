LIST =		funlist_default.o funlist_list.o funlist_util.o
APMATH =	apmath_memory.o apmath_indecrement.o	\
		apmath_addition.o apmath_subtraction.o apmath.o
FLAG =	-O3


all: $(APMATH) $(LIST) angel.o
	cc $(FLAG) *.o

clean:
	rm -f *.out *.o
	rm -f y.tab* *.output
	rm -rf html latex

funlist_default.o: funlist_default.c
	cc $(FLAG) -c funlist_default.c
funlist_list.o: funlist_list.c
	cc $(FLAG) -c funlist_list.c
funlist_util.o: funlist_util.c
	cc $(FLAG) -c funlist_util.c

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

program:
	bison -d -y -v grammer.y
	flex parse.l
	cc y.tab.c lex.yy.c
