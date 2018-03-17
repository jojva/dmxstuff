all: aseqdump dmxd dmxdummy dmxtest

dmxd: dmxd.c
	gcc -o dmxd dmxd.c -lusb

aseqdump: aseqdump.c
	gcc -o aseqdump aseqdump.c -lasound

dmxdummy: dmxdummy.c
	gcc -o dmxdummy dmxdummy.c -lusb

dmxtest: dmxtest.c
	gcc -o dmxtest dmxtest.c

clean:
	rm -f *.o aseqdump dmxtest dmxd
