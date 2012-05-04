all: ../bin/sfn

../bin/sfn: sfn.c
	mkdir -p ../bin
	gcc -Wall -O3 \
		-I ../lib/argtable2-13/src \
		../lib/argtable2-13/src/*.c $? -o $@

.PONY: all

