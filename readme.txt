Setup Faerie project

(I.) Download Windows:
- WinSCP - Manipulate files on RPi from windows
- VisualGDB - Debug from visual studio
- GitHub Windows

(II.) Download Linux
- maybe alsa if not installed
- espeak
	- libportaudio
	- libespeak
	- sudo apt-get install espeak
	- choose the right portaudio header and rename to portaudio.h //maybe not needed
	- do we need to setup a link for the program to find the library?
- voce

(III.) Linking in Java Virtual Machine (example of adding dynamic library dependancies):
- As long as we still use Voce we need the Library for the JVM
- Maybe we will also need to do this for espeak (forgot whether I did that)
1 Locate your JDK installation, Raspberrian should come with one (hopefully jdk-8)
2 Somewhere around here: /usr/lib/jvm/jdk-8-oracle-arm32-vfp-hflt/
3 There you search where you can find libjvm.so (I found one here: /usr/lib/jvm/jdk-8-oracle-arm32-vfp-hflt/jre/lib/arm/server)
4 As you can see in (V.) we use the library jvm, for Linux to find it we need to add it to a path where it finds it, for that we use a softlink(so we dont copy the file). A path where Linux looks for libraries is '/usr/lib'. With the setup in (V.) we actually do not need to add the softlink cause we add all the paths in library dir and LD_LIBRARY_PATH(first is for statical linking lookups, the second one for runtime). But if we want to run Faerie in stand alone (not from VS) we need the softlink (maybe there is also another way like making a bash file that starts the Faerie executeable with the additional LD_LIBRARY_PATH).
5 'ln -sf /usr/lib/jvm/jdk-8-oracle-arm32-vfp-hflt/jre/lib/arm/server/libjvm.so /usr/lib/libjvm.so' will do the trick. ATTENTION! Your path may differ!!!
6. You can read up on this here: http://www.yolinux.com/TUTORIALS/LibraryArchives-StaticAndDynamic.html

(IV.) Configuring your Microphone for Pulse Audio:
1 Open Terminal
2 Execute 'pacmd'
3 Now type: 'list-sources'
4 Find your microphone and copy its name
5 Set it temporarily by typing 'set-default-source [name]''
6 Try it out:
	- open another terminal
	- navigate to your home directory
	- type 'arecord -f cd test.wav' and say something
	- type 'aplay test.wav' - did you hear yourself? good.
7 Check whether there is a folder called .pulse in your home directory ('ls -a')
	- If not: Execute 'mkdir .pulse'
8 Check whether there is a file called default.pa in the folder
	- If not: Open File Browser go to '/etc/pulse/default.pa' and copy and paste it into the folder
9 Open the 'default.pa' in the '~/.pulse' folder and search for a line with 'set-default-source'
10 Remove the hashtag if there is one
11 Whatever it says behind 'set-default-source' remove it and paste the name of the device you copied earlier (step 4)
12 Save file
13 Reboot
14 Execute step 6 again. Did it work? Good.


(V.) VGDB Setup:
	1 New Linux Project
	2 Compile on Remote machine
	3 Makefile:
	- cflags: -ggdb -ffunction-sections -O0 -std=gnu++11
	- libraries: espeak jvm
	- Include dir: /usr/lib/jvm/jdk-8-oracle-arm32-vfp-hflt/include /usr/lib/jvm/jdk-8-oracle-arm32-vfp-hflt/include/linux
	- library dir: /usr/lib/jvm/jdk-8-oracle-arm32-vfp-hflt/lib /usr/lib/jvm/jdk-8-oracle-arm32-vfp-hflt/jre/lib /usr/lib/jvm/jdk-8-oracle-arm32-vfp-hflt/jre/lib/arm/client /usr/lib/jvm/jdk-8-oracle-arm32-vfp-hflt/jre/lib/arm/server
	4 Debugging:
	- LD_LIBRARY_PATH: /usr/lib/jvm/jdk-8-oracle-arm32-vfp-hflt/jre/lib/arm/client:/usr/lib/jvm/jdk-8-oracle-arm32-vfp-hflt/jre/lib/arm/server