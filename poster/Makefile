BIN=../bin/poster

all: build

build: $(BIN)
	rsync -av ./* $(BIN)
	make -C $(BIN) _build

_build:
	TEXINPUTS=.:../../lib//: latexmk -pdf group-2-poster.tex

$(BIN):
	mkdir -p $@

.PHONY: build _build

