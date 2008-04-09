VLC Phonon backend uses libvlc-0.9 from VLC

* Under Windows
You need:
- Visual C++ 2005
- Platform SDK
- DirectX SDK
- Qt 4.4.0 beta1
- CMake 2.4.8
Everything properly configured with INCLUDE, LIB, QTDIR... environment variables

Using a prompt, go to directory build:
build_nmake-debug.bat
nmake
nmake install


nmake install will copy phonon_vlc.dll inside directory C:\Qt\4.4.0\plugins\phonon_backend

You have to clean the directory phonon_backend otherwise phonon_ds9 backend will be used
(Qt loads backend by alphabetic order and does not allow to choose the backend used)
