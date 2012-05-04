all: build

build:
	make -C src/

clean:
	rm -fr bin/

rebuild: clean build

.PONY: all build clean

