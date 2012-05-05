
ARGTABLE = argtable2-13

all: $(ARGTABLE) baposter.cls

clean:
	rm -fr $(ARGTABLE)
	rm -f baposter.cls

$(ARGTABLE):
	wget -O - http://prdownloads.sourceforge.net/argtable/$@.tar.gz | tar -xz
	(cd $@ && ./configure && make)

baposter.cls:
	wget http://www.brian-amberg.de/uni/poster/baposter/baposter.cls

.PHONY: all clean

