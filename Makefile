all: build poster

build: lib
	make -C src/

clean:
	rm -fr bin/
	make -C src/ clean

clean-deep: clean
	make -C lib/ clean

lib:
	make -C lib

poster:
	make -C poster

rebuild: clean build

rebuild-deep: clean-deep build

test: build
	valgrind ./bin/sfn/sfn -d bin/sfn/sfn.dot -a bin/sfn/animation \
	    -n 10 -m 3 -T 10 -s 4 -k 100

test-graph: test
	dot -T png -o bin/sfn/sfn.png bin/sfn/sfn.dot
	eog bin/sfn/sfn.png

.PHONY: all build clean clean-deep lib poster rebuild rebuild-deep test \
		test-build

