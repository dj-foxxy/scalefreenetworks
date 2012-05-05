ARGTABLE = argtable2-13

all: $(ARGTABLE) baposter.cls pygraphviz-1.1

clean:
	rm -fr $(ARGTABLE) pygraphviz-1.1 baposter.cls

$(ARGTABLE):
	wget -O - http://switch.dl.sourceforge.net/project/argtable/argtable/argtable-2.13/$@.tar.gz | tar -xz
	(cd $@ && ./configure && make)

baposter.cls:
	wget http://www.brian-amberg.de/uni/poster/baposter/baposter.cls

pygraphviz-1.1:
	wget -O - http://pypi.python.org/packages/source/p/pygraphviz/$@.tar.gz \
		| tar -xz
	(cd $@ && python2 setup.py install --user)

.PHONY: all clean

