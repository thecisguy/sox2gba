# sox2gba
Uses sox to convert any audio file to a C WaveData structure for use in GBA programming.

When adding digital sounds to a GBA program, it is necessary to convert it to
8-bit signed PCM audio at some bitrate. This data needs to be encoded in the
executable ROM itself in order to be read. The easiest way to do that is to use the
WaveData structure, which includes some simple information about the sound
along with the data itself, and is directly useable with the GBA BIOS sound driver.

While many homebrew tools already exist, they vary wildly with respect to their
supported input formats, development tools required, supported operating systems,
etc. This application attempts to improve on these by utilizing sox, "the Swiss Army
knife of sound processing programs." It uses POSIX C, and so it should compile and run
on any POSIX-compliant OS, or on MS Windows by using Cygwin. Obviously, you will
also need a version of sox for your OS.

Sox is used to convert the audio, and therefore this program supports any input
file format that sox does. For this program to work, sox must be in the PATH.
Basically this means having sox in /usr/bin (which should be the default location
after installing it), or having sox.exe in the same folder as this program on MS Windows.

The output of this program is a ready-to-compile C source file which contains
the sound data, as well as the other WaveData data. This structure is ready
to be used with the GBA BIOS sound driver functions; see libgba's
<gba_sound.h> header for a useful interface to those.

Visit http://sox.sourceforge.net/ to learn more about sox and download it.
