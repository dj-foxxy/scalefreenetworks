
visual_sources = src/*.cpp
visual_objects = *.o

V_CFLAGS= -I/usr/X11R6/include -I/usr/local/include -Wall
V_LDFLAGS= -ldl

OGL_CFLAGS=-DGL_GLEXT_PROTOTYPES
OGL_LDFLAGS=-lGL -lglut -lGLU

all: build

meatball: $(visual_objects)
	g++ $(LIBFLAGS) $(OGL_CFLAGS) -o bin/meatball $(visual_objects) $(LDFLAGS) $(OGL_LDFLAGS)
	rm $(visual_objects)

build: lib
	make -C src/

clean:
	rm -fr bin/

clean-deep: clean
	make -C lib/ clean

lib:
	make -C lib

rebuild: clean build

test: build
	valgrind ./bin/sfn -d bin/sfn.dot -n 10 -m 3 -T 10 -s 4 -k 100
	dot -T png -o bin/sfn.png bin/sfn.dot
	eog bin/sfn.png

.PONY: all build clean clean-deep lib test


$(visual_objects): $(visual_sources)
	g++ -c $(CFLAGS) $(visual_sources)
