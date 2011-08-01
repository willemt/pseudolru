CC     = gcc
CCFLAGS = -g


all: tests

main.c:
	sh make-tests.sh > main.c

tests: main.c pseudolru.o test_pseudolru.c CuTest.c main.c
	$(CC) $(CCFLAGS) -o $@ $^
	./tests

pseudolru.o: pseudolru.c
	$(CC) $(CCFLAGS) -c -o $@ $^

clean:
	rm -f main.c pseudolru.o tests
