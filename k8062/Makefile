all: sinewaves blackout whiteout

blackout: k8062.o blackout.cpp
	g++ -o blackout blackout.cpp k8062.o

whiteout: k8062.o whiteout.cpp
	g++ -o whiteout whiteout.cpp k8062.o

sinewaves: k8062.o sinewaves.cpp
	g++ -o sinewaves sinewaves.cpp k8062.o -lm

k8062.o: k8062.cpp k8062.h
	g++ -c k8062.cpp

clean:
	rm -f *.o sinewaves
