# Synesthesizer
Synesthesizer, a MIDI to DMX converter based on fredex42/dmxstuff and aseqdump

------------
How to install:

1. Install required libraries `# apt install libusb-1.0-0-dev libasound2-dev`
2. Build DMX daemon with `$ make` from k8062/k8062forlinux/
3. Run DMX daemon with `$ ./dmxd` from k8062/k8062forlinux/
4. From another terminal, build synesthesizer with `$ make` from k8062/
5. Run synesthesizer with `$ ./synesthesizer [options]` from k8062/

------------
Based on:
* https://github.com/fredex42/dmxstuff/ for the DMX part
* https://github.com/bear24rw/alsa-utils/blob/master/seq/aseqdump/aseqdump.c for the MIDI part
