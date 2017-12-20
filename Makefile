CFLAGS = -std=c99 -g -Wall -Wshadow --pedantic -Wvla -Werror -pthread
COVFLAGS = -fprofile-arcs -ftest-coverage
#PROFFLAG = -pg
#DEFINES = -DTEST_READ -DTEST_WRITE -DTEST_SORTID -DTEST_SORTFIRSTNAME -DTEST_SORTLASTNAME
VALGRIND = valgrind --tool=memcheck --leak-check=full
VALGRIND += --verbose --track-origins=yes --leak-check=full --show-leak-kinds=all  --log-file=
GCC = gcc $(CFLAGS) $(COVFLAGS) #$(DEFINES)
#OBJS = pa14.o 

all: pa14

pa14: bmp.o filterParallel.o pa14.o
	$(GCC) pa14.o bmp.o filterParallel.o -o pa14 -lm

pa14.o: pa14.c filterParallel.h bmp.h
	$(GCC) -c pa14.c -o pa14.o

bmp.o: bmp.c bmp.h 
	$(GCC) -c bmp.c -o bmp.o

filterParallel.o: filterParallel.c filterParallel.h bmp.h
	$(GCC) -c filterParallel.c -o filterParallel.o

test: pa14
	  echo "Testing filter" ; \
	./pa14 test.bmp out.bmp
inspect: pa14
	./pa14 test.bmp out.bmp
	gcov pa14.c
	gcov bmp.c
	gcov filterParallel.c

memory: pa14
	$(VALGRIND)memory ./pa14 test.bmp out.bmp
clean: 
	/bin/rm -f *.o
	/bin/rm -f testgen
	/bin/rm -f *.gcda *.gcno gmon.out *gcov
	/bin/rm -f pa14
	/bin/rm -f memory
	/bin/rm -f validate
	/bin/rm -f out.bmp
