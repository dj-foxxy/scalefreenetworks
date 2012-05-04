all: build

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

.PONY: all build clean deep-clean

