MPlayer Phonon backend uses libsmplayer from SMPlayer
it calls methods from libsmplayer::Core

* Under Windows
You need:
- Visual C++ 2005
- Platform SDK
- DirectX SDK
- Qt 4.4.0 beta1
- CMake 2.4.8

After compiling phonon_mplayer.dll using CMake, you must copy-paste it to
C:\Qt\4.4.0\plugins\phonon_backend
after having renaming the previous phonon_backend directory otherwise phonon_ds9 backend will be used
(Qt loads backend by alphabetic order and does not allow to choose the backend used)
