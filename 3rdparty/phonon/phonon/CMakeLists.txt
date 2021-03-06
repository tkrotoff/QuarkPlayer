project(phonon)

set(PHONON_LIB_MAJOR_VERSION "4")
set(PHONON_LIB_MINOR_VERSION "4")
set(PHONON_LIB_PATCH_VERSION "3")
set(PHONON_LIB_VERSION "${PHONON_LIB_MAJOR_VERSION}.4.0")
set(PHONON_LIB_SOVERSION ${PHONON_LIB_MAJOR_VERSION})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

option(PHONON_NO_CAPTURE "Disable the capture API" ON)
if (PHONON_NO_CAPTURE)
    add_definitions(-DQT_NO_PHONON_VIDEOCAPTURE -DQT_NO_PHONON_AUDIOCAPTURE)
endif (PHONON_NO_CAPTURE)

if (PHONON_BUILD_TESTS)
    add_subdirectory(tests)
endif (PHONON_BUILD_TESTS)

#add_subdirectory(experimental)

set(PULSEAUDIO_MINIMUM_VERSION "0.9.15")
#macro_optional_find_package(PulseAudio)
if (PULSEAUDIO_FOUND)
    # PULSEAUDIO_DEVICE_MANAGER feature check could be moved to FindPulseAudio.cmake, hint hint. -- Rex
    macro_ensure_version("0.9.21" "${PULSEAUDIO_VERSION}" PULSEAUDIO_DEVICE_MANAGER)
endif (PULSEAUDIO_FOUND)
#macro_log_feature(PULSEAUDIO_FOUND "PulseAudio" "A cross-platform, networked sound server." "http://www.pulseaudio.org" FALSE "" "Allows audio playback via the PulseAudio soundserver when it is running")
#macro_optional_find_package(GLIB2)
#macro_log_feature(GLIB2_FOUND "GLib2" "GLib 2 is required to compile the pulseaudio for Phonon" "http://www.gtk.org/download/" FALSE)


if (GLIB2_FOUND AND PULSEAUDIO_FOUND)
   add_definitions(-DHAVE_PULSEAUDIO)
   include_directories(${GLIB2_INCLUDE_DIR} ${PULSEAUDIO_INCLUDE_DIR})
   if (PULSEAUDIO_DEVICE_MANAGER)
      add_definitions(-DHAVE_PULSEAUDIO_DEVICE_MANAGER)
   endif(PULSEAUDIO_DEVICE_MANAGER)
else(GLIB2_FOUND AND PULSEAUDIO_FOUND)
   set(PULSEAUDIO_INCLUDE_DIR "")
   set(PULSEAUDIO_LIBRARY "")
   set(PULSEAUDIO_MAINLOOP_LIBRARY "")
endif(GLIB2_FOUND AND PULSEAUDIO_FOUND)


set(phonon_LIB_SRCS
   objectdescription.cpp
   objectdescriptionmodel.cpp
   phononnamespace.cpp
   mediasource.cpp
   abstractmediastream.cpp
   streaminterface.cpp
   mediaobject.cpp
   medianode.cpp
   path.cpp
   effectparameter.cpp
   effect.cpp
   volumefadereffect.cpp
   abstractaudiooutput.cpp
   abstractaudiooutput_p.cpp
   audiooutput.cpp
   audiooutputinterface.cpp
   abstractvideooutput.cpp
   abstractvideooutput_p.cpp
   backendcapabilities.cpp
   globalconfig.cpp
   factory.cpp
   platform.cpp
   mediacontroller.cpp
   videowidget.cpp
   videoplayer.cpp
   seekslider.cpp
   swiftslider.cpp
   volumeslider.cpp
   effectwidget.cpp
   iodevicestream.cpp
   audiodataoutput.cpp
   pulsesupport.cpp
   )

if (PULSEAUDIO_FOUND)
   list(APPEND phonon_LIB_SRCS
      pulsestream.cpp
   )
endif (PULSEAUDIO_FOUND)

if (QT_QTDBUS_FOUND)
   list(APPEND phonon_LIB_SRCS
      audiooutputadaptor.cpp
   )
endif (QT_QTDBUS_FOUND)

option(PHONON_NO_PLATFORMPLUGIN "Do not use any platform plugin")
if (PHONON_NO_PLATFORMPLUGIN)
    add_definitions(-DQT_NO_PHONON_PLATFORMPLUGIN)
endif (PHONON_NO_PLATFORMPLUGIN)

add_definitions(-DPHONON_LIBRARY_PATH="${PLUGIN_INSTALL_DIR}/plugins")

qt4_automoc(${phonon_LIB_SRCS})
add_library(phonon SHARED ${phonon_LIB_SRCS})
target_link_libraries(phonon ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})
if (GLIB2_FOUND AND PULSEAUDIO_FOUND)
target_link_libraries(phonon ${GLIB2_LIBRARIES} ${GOBJECT_LIBRARIES} ${PULSEAUDIO_LIBRARY} ${PULSEAUDIO_MAINLOOP_LIBRARY})
endif (GLIB2_FOUND AND PULSEAUDIO_FOUND)

if (QT_QTDBUS_FOUND)
  target_link_libraries(phonon ${QT_QTDBUS_LIBRARY})
endif (QT_QTDBUS_FOUND)
if (${CMAKE_SYSTEM_NAME} MATCHES "SunOS")
  # We need to explicitly link libm to phonon in Solaris
  target_link_libraries(phonon m)
endif (${CMAKE_SYSTEM_NAME} MATCHES "SunOS")

set_target_properties(phonon PROPERTIES
                             VERSION ${PHONON_LIB_VERSION}
                             SOVERSION ${PHONON_LIB_SOVERSION}
                             DEFINE_SYMBOL MAKE_PHONON_LIB
                             )
install(TARGETS phonon ${INSTALL_TARGETS_DEFAULT_ARGS})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/phononnamespace.h.in ${CMAKE_CURRENT_SOURCE_DIR}/phononnamespace.h)
