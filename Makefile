CFLAGS = -Wall -Werror -g
CC     = gcc $(CFLAGS)
SHELL  = /bin/bash
CWD    = $(shell pwd | sed 's/.*\///g')

PROGRAMS = \
	batter_main \


all : $(PROGRAMS)

clean :
	rm -f $(PROGRAMS) *.o

AN=a2
zip :
	rm -f $(AN)-code.zip
	cd .. && zip "$(CWD)/$(AN)-code.zip" -r "$(CWD)"
	@echo Zip created in $(AN)-code.zip
	@if (( $$(stat -c '%s' $(AN)-code.zip) > 10*(2**20) )); then echo "WARNING: $(AN)-code.zip seems REALLY big, check there are no abnormally large test files"; du -h $(AN)-code.zip; fi

# battery problem
batter_main : batter_main.o batter_update.o batter_sim.o
	$(CC) -o $@ $^

batter_main.o : batter_main.c batter.h
	$(CC) -c $<

batter_sim.o : batter_sim.c batter.h
	$(CC) -c $<

batter_update.o : batter_update.c batter.h
	$(CC) -c $<

# Testing Targets
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

test: test-p1 test-p2

test-p1: test-p1a test-p1b

test-p1a: test_batter_update
	@chmod u+rx test_batter_update.sh
	./test_batter_update.sh

test-p1b : batter_main
	@chmod u+rx test_batter_main.sh
	./test_batter_main.sh

test_batter_update : test_batter_update.o batter_sim.o batter_update.o
	$(CC) -o $@ $^

clean-tests : clean
	rm -f test-data/*.tmp test_batter_update
