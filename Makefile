all: build test


visual_sources = src/*.cpp
visual_objects = *.o

V_CFLAGS= -I/usr/X11R6/include -I/usr/local/include -Wall
V_LDFLAGS= -ldl

OGL_CFLAGS=-DGL_GLEXT_PROTOTYPES
OGL_LDFLAGS=-lGL -lglut -lGLU

#compile
meatball: $(visual_objects)
	g++ $(LIBFLAGS) $(OGL_CFLAGS) -o bin/meatball $(visual_objects) $(LDFLAGS) $(OGL_LDFLAGS)
	rm $(visual_objects)

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
	valgrind ./bin/sfn -d bin/sfn.dot -n 10 -m 3 -T 1000 -s 4 -k 10
	dot -T png -o bin/sfn.png bin/sfn.dot
	eog bin/sfn.png

.PONY: all build clean deep-clean lib test


$(visual_objects): $(visual_sources)
	g++ -c $(CFLAGS) $(visual_sources)
