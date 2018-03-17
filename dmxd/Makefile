all: dmxd dmxdummy dmxtest

dmxd: dmxd.c
	gcc -o dmxd dmxd.c -lusb

dmxdummy: dmxdummy.c
	gcc -o dmxdummy dmxdummy.c -lusb

dmxtest: dmxtest.c
	gcc -o dmxtest dmxtest.c

clean:
	rm -f *.o dmxtest dmxd
