CFLAGS=-Wall -Wextra -Werror -Wno-unused

all: sinewaves blackout whiteout synesthesizer

aseqdump.o: aseqdump.c aseqdump.h
	gcc $(CFLAGS) -c aseqdump.c -lasound

adsr.o: adsr.cpp adsr.h
	gcc $(CFLAGS) -c adsr.cpp -lasound

note.o: adsr.h note.cpp note.h
	gcc $(CFLAGS) -c note.cpp -lasound

synesthesizer.o: note.h synesthesizer.cpp synesthesizer.h
	g++ $(CFLAGS) -c synesthesizer.cpp $(CPPFLAGS) -lasound

blackout: k8062.o blackout.cpp
	g++ $(CFLAGS) -o blackout blackout.cpp k8062.o

whiteout: k8062.o whiteout.cpp
	g++ $(CFLAGS) -o whiteout whiteout.cpp k8062.o

sinewaves: k8062.o sinewaves.cpp
	g++ $(CFLAGS) -o sinewaves sinewaves.cpp k8062.o -lm

synesthesizer: k8062.o aseqdump.o note.o adsr.o synesthesizer.o main.cpp
	g++ $(CFLAGS) -o synesthesizer main.cpp synesthesizer.o k8062.o aseqdump.o note.o adsr.o -lm -lasound

k8062.o: k8062.cpp k8062.h
	g++ $(CFLAGS) -c k8062.cpp

clean:
	rm -f *.o sinewaves synesthesizer
