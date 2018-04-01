CFLAGS=-Wall -Wextra -Werror -Wno-unused

all: sinewaves blackout whiteout synesthesizer

aseqdump.o: aseqdump.c aseqdump.h
	gcc $(CFLAGS) -c aseqdump.c -lasound

synesthesizer.o: synesthesizer.cpp synesthesizer.h
	g++ $(CFLAGS) -c synesthesizer.cpp $(CPPFLAGS) -lasound -lSDL2

blackout: k8062.o blackout.cpp
	g++ $(CFLAGS) -o blackout blackout.cpp k8062.o

whiteout: k8062.o whiteout.cpp
	g++ $(CFLAGS) -o whiteout whiteout.cpp k8062.o

sinewaves: k8062.o sinewaves.cpp
	g++ $(CFLAGS) -o sinewaves sinewaves.cpp k8062.o -lm

synesthesizer: k8062.o aseqdump.o synesthesizer.o main.cpp
	g++ $(CFLAGS) -o synesthesizer main.cpp synesthesizer.o k8062.o aseqdump.o -lm -lasound -lSDL2

k8062.o: k8062.cpp k8062.h
	g++ $(CFLAGS) -c k8062.cpp

clean:
	rm -f *.o sinewaves synesthesizer
