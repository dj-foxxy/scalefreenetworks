BIN=../../bin/vis
OBJS := $(patsubst %.cpp,%.o,$(wildcard *.cpp))
OGL_CFLAGS = -DGL_GLEXT_PROTOTYPES
OGL_LDFLAGS = -lGL -lglut -lGLU
V_CFLAGS= -I/usr/X11R6/include -I/usr/local/include -Wall
V_LDFLAGS= -ldl

all: build

build: $(BIN)/vis

clean:
	rm -f *.o

$(BIN)/vis: $(BIN) $(OBJS)
	g++ $(LIBFLAGS) $(OGL_CFLAGS) -o $@ $(OBJS) $(LDFLAGS) $(OGL_LDFLAGS)

$(BIN):
	mkdir -p $@

.PHONY: all build

