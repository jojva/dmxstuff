# Synesthesizer
Synesthesizer, a MIDI to DMX converter based on fredex42/dmxstuff and aseqdump

------------
How to install:

1. Install required libraries `# apt install libusb-dev libusb-1.0-0-dev libasound2-dev qt5-default`
2. Install Qt's charts library `# apt install libqt5charts5-dev`. If it doesn't work, see "How to install Qt charts"
3. Build DMX daemon with `$ make` from dmxd/
4. Run DMX daemon with `$ ./dmxd` from dmxd/
5. From another terminal, build synesthesizer with `$ qmake` then `$ make` from src/
6. Run synesthesizer with `$ ./synesthesizer [options]` from src/

------------
How to install Qt charts

Qt5 charts is a pretty recent library (as of April 2018), and may not be available on your system (such as a raspbian). In this case, we have to build it ourselves:

1. `$ git clone https://github.com/qt/qtcharts.git`
2. `$ cd qtcharts`
3. `$ git checkout v5.7.1`
4. `$ qmake`
5. `$ make` # This may take a long time on slow hardware like a raspberry pi
6. `# make install`

------------
Based on:
* https://github.com/fredex42/dmxstuff/ for the DMX part
* https://github.com/bear24rw/alsa-utils/blob/master/seq/aseqdump/aseqdump.c for the MIDI part
