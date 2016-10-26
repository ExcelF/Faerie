Setup Faerie project

Download Windows:
- WinSCP - Manipulate files on RPi from windows
- VisualGDB - Debug from visual studio
- GitHub Windows

Download Linux
- gnome-audio-recorder - for microphone tests
- maybe alsa if not installed
- espeak
	- libportaudio
	- libespeak
	- sudo apt-get install espeak
	- choose the right portaudio header and rename to portaudio.h //maybe not needed
	- do we need to setup a link for the program to find the library?
- voce

VGDB Setup:
	- New Linux Project
	- Compile on Remote machine
	Makefile:
	- cflags: -ggdb -ffunction-sections -O0 -std=gnu++11
	- libraries: espeak jvm
	- Include dir: /usr/lib/jvm/jdk-8-oracle-arm32-vfp-hflt/include /usr/lib/jvm/jdk-8-oracle-arm32-vfp-hflt/include/linux
	- library dir: /usr/lib/jvm/jdk-8-oracle-arm32-vfp-hflt/lib /usr/lib/jvm/jdk-8-oracle-arm32-vfp-hflt/jre/lib /usr/lib/jvm/jdk-8-oracle-arm32-vfp-hflt/jre/lib/arm/client /usr/lib/jvm/jdk-8-oracle-arm32-vfp-hflt/jre/lib/arm/server
	Debugging:
	- LD_LIBRARY_PATH: /usr/lib/jvm/jdk-8-oracle-arm32-vfp-hflt/jre/lib/arm/client:/usr/lib/jvm/jdk-8-oracle-arm32-vfp-hflt/jre/lib/arm/server