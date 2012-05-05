BIN=../../bin/sfn
LIB=../../lib

all: build

build: $(BIN) $(BIN)/sfn

$(BIN):
	mkdir -p $@

$(BIN)/sfn: sfn.c
	gcc -Wall -Werror -g0 -Ofast --std=c99 \
		-I $(LIB)/argtable2-13/src -lm \
		$(LIB)/argtable2-13/src/*.o $? -o $@

.PHONY: all build

