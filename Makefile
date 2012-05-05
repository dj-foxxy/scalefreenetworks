all: build

build: lib
	make -C src/

clean:
	rm -fr bin/
	make -C src/ clean

clean-deep: clean
	make -C lib/ clean

lib:
	make -C lib

rebuild: clean build

rebuild-deep: clean-deep build

test: build
	valgrind ./bin/sfn/sfn -d bin/sfn/sfn.dot -n 10 -m 3 -T 10 -s 4 -k 100
	dot -T png -o bin/sfn/sfn.png bin/sfn/sfn.dot
	eog bin/sfn/sfn.png

.PHONY: all build clean clean-deep lib rebuild rebuild-deep test

