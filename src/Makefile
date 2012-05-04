all: ../bin/sfn

../bin/sfn: sfn.c
	mkdir -p ../bin
	gcc -Wall -Werror --std=c99 -O3 $? -o $@

.PONY: all

