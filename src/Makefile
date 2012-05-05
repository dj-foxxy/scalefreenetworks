all: build

build: sfn vis

clean:
	make -C vis/ clean

sfn:
	make -C $@/

vis:
	make -C $@/

.PHONY: all build clean sfn vis

