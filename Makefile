all: build test

build: lib
	make -C src/

clean:
	rm -fr bin/

deep-clean: clean
	rm -fr lib/

lib:
	mkdir -p lib/
	wget -O - http://prdownloads.sourceforge.net/argtable/argtable2-13.tar.gz \
		| tar --directory=lib/ -xz
	(cd lib/argtable2-13 && ./configure && make)

rebuild: clean build

test: build
	valgrind ./bin/sfn -d bin/sfn.dot -n 10 -m 3 -T 1000 -s 4
	dot -T png -o bin/sfn.png bin/sfn.dot
	eog bin/sfn.png

.PONY: all build clean deep-clean lib test

