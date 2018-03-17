# Synesthesizer
Synesthesizer, a MIDI to DMX converter based on fredex42/dmxstuff and aseqdump

------------
How to install:

1. Install required libraries `# apt install libusb-1.0-0-dev libasound2-dev`
2. Build DMX daemon with `$ make` from dmxd/
3. Run DMX daemon with `$ ./dmxd` from dmxd/
4. From another terminal, build synesthesizer with `$ make` from src/
5. Run synesthesizer with `$ ./synesthesizer [options]` from src/

------------
Based on:
* https://github.com/fredex42/dmxstuff/ for the DMX part
* https://github.com/bear24rw/alsa-utils/blob/master/seq/aseqdump/aseqdump.c for the MIDI part
