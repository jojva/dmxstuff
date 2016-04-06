all: dmxinterface.pm dmxinterface.so

dmxinterface_wrap.c: dmxinterface.i
	swig -perl5 dmxinterface.i

dmxinterface.pm: dmxinterface.i
	swig -perl5 dmxinterface.i

dmxinterface.so: dmxinterface_wrap.c dmxinterface.c
	gcc -shared -g -o dmxinterface.so dmxinterface.c -Dbool=char -DDEBUG \
`perl -MExtUtils::Embed -e ccopts` -Dbool=char dmxinterface_wrap.c

clean:
	rm -f dmxinterface.pm dmxinterface.so dmxinterface_wrap.c
