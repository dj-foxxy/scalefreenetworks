all: build

build: ../bin/sfn

../bin/sfn: sfn.c
	mkdir -p ../bin
	gcc -Wall -g3 -O0 --std=c99 \
		-I ../lib/argtable2-13/src -lm\
		../lib/argtable2-13/src/*.c $? -o $@

.PONY: all build test

