CPPFLAGS=-Wall -Wextra

all: sinewaves blackout whiteout synesthesizer

aseqdump.o: aseqdump.c aseqdump.h
	gcc -c aseqdump.c -lasound

synesthesizer.o: synesthesizer.cpp synesthesizer.h
	g++ -c synesthesizer.cpp $(CPPFLAGS) -lasound

blackout: k8062.o blackout.cpp
	g++ -o blackout blackout.cpp k8062.o

whiteout: k8062.o whiteout.cpp
	g++ -o whiteout whiteout.cpp k8062.o

sinewaves: k8062.o sinewaves.cpp
	g++ -o sinewaves sinewaves.cpp k8062.o -lm

synesthesizer: k8062.o aseqdump.o synesthesizer.o main.cpp
	g++ -o synesthesizer main.cpp synesthesizer.o k8062.o aseqdump.o $(CPPFLAGS) -lm -lasound

k8062.o: k8062.cpp k8062.h
	g++ -c k8062.cpp

clean:
	rm -f *.o sinewaves synesthesizer
