
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
all:

.PONY: all


$(visual_objects): $(visual_sources)
	g++ -c $(CFLAGS) $(visual_sources)
