
ARGTABLE = argtable2-13

all: $(ARGTABLE)

clean:
	rm -fr $(ARGTABLE)

$(ARGTABLE):
	wget -O - http://prdownloads.sourceforge.net/argtable/$@.tar.gz | tar -xz
	(cd $@ && ./configure && make)

.PONY: all clean

