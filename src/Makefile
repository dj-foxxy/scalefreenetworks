all: build test

build: ../bin/sfn

test: build
	../bin/sfn -n 1 -m 3 -T 10

../bin/sfn: sfn.c
	mkdir -p ../bin
	gcc -Wall -O3 \
		-I ../lib/argtable2-13/src \
		../lib/argtable2-13/src/*.c $? -o $@

.PONY: all build test

