project(phonon)

find_package(KDE4)

if (NOT KDE4_FOUND)
  set(CMAKE_MODULE_PATH
    ${CMAKE_SOURCE_DIR}/cmake/modules
  )
  include(KDE4Defaults)

  find_package(Qt4 REQUIRED)
  include_directories(${QT_INCLUDE_DIR})
  add_definitions(${QT_DEFINITIONS})

  set(SHARE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}/share")
  set(KDE4_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}")
  set(DBUS_INTERFACES_INSTALL_DIR "${KDE4_INSTALL_DIR}/share/dbus-1/interfaces" )
  set(SERVICETYPES_INSTALL_DIR "${SHARE_INSTALL_PREFIX}/servicetypes")

  message(STATUS "KDE4 not found")
endif (NOT KDE4_FOUND)

FIND_PACKAGE(Alsa)
ALSA_CONFIGURE_FILE(${CMAKE_CURRENT_BINARY_DIR}/config-alsa.h)

OPTION(KDE4_DISABLE_MULTIMEDIA "Disable multimedia support (default: off)" OFF)
IF (KDE4_DISABLE_MULTIMEDIA)
  MESSAGE(STATUS "Phonon multimedia support disabled by user.")
  SET(HAVE_KDE4_MULTIMEDIA 0)
ELSE (KDE4_DISABLE_MULTIMEDIA)
  MESSAGE(STATUS "Phonon multimedia support is enabled.")
  SET(HAVE_KDE4_MULTIMEDIA 1)
ENDIF (KDE4_DISABLE_MULTIMEDIA)

configure_file(config-phonon.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-phonon.h )

if (KDE4_FOUND)
  # Those apply to all subdirs
  include_directories(${KDE4_KDECORE_INCLUDES} ${CMAKE_BINARY_DIR}/solid)
endif (KDE4_FOUND)

#Phonon detection, this should be integrated into FindQt4.cmake
find_library(QT_PHONON_LIBRARY_RELEASE NAMES phonon phonon4 PATHS ${QT_LIBRARY_DIR} NO_DEFAULT_PATH)
find_library(QT_PHONON_LIBRARY_DEBUG NAMES phonon_debug phonond4 PATHS ${QT_LIBRARY_DIR} NO_DEFAULT_PATH)
_QT4_ADJUST_LIB_VARS(PHONON)
##

#add_subdirectory(tests)
#add_subdirectory(examples)
#add_subdirectory(experimental)
#add_subdirectory(platform_kde)
#add_subdirectory(libkaudiodevicelist)
if(NOT MINGW)
  # ICE in outputdevicechoice.cpp:262
  #add_subdirectory( kcm )
endif(NOT MINGW)
########### next target ###############

set(phonon_LIB_SRCS
   objectdescription.cpp
   objectdescriptionmodel.cpp
   phononnamespace.cpp
   mediasource.cpp
   abstractmediastream.cpp
   abstractmediastream2.cpp
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
   volumeslider.cpp
   effectwidget.cpp
   iodevicestream.cpp
   lockfreequeue.cpp
   streameventqueue.cpp
   )

if (QT_QTDBUS_FOUND)
  set(phonon_LIB_SRCS
    ${phonon_LIB_SRCS}
      audiooutputadaptor.cpp
  )
endif (QT_QTDBUS_FOUND)

if (KDE4_FOUND)
  kde4_add_library(phonon SHARED ${phonon_LIB_SRCS})
else (KDE4_FOUND)
  add_definitions(-DMAKE_PHONON_LIB)
  qt4_automoc(${phonon_LIB_SRCS})
  add_library(phonon SHARED ${phonon_LIB_SRCS})
endif (KDE4_FOUND)

if (QT_QTDBUS_FOUND)
  target_link_libraries(phonon ${QT_QTDBUS_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})
else (QT_QTDBUS_FOUND)
  target_link_libraries(phonon ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})
endif (QT_QTDBUS_FOUND)

#do not use GENERIC versioning in phonon
set_target_properties(phonon PROPERTIES VERSION 4.2.0 SOVERSION 4)
install(TARGETS phonon ${INSTALL_TARGETS_DEFAULT_ARGS})

########### install files ###############

install( FILES
   phonon_export.h
   objectdescription.h
   objectdescriptionmodel.h
   phononnamespace.h
   mediasource.h
   abstractmediastream.h
   abstractmediastream2.h
   streaminterface.h
   mediaobject.h
   audiooutput.h
   medianode.h
   path.h
   effectparameter.h
   effect.h
   effectinterface.h
   volumefadereffect.h
   volumefaderinterface.h
   abstractaudiooutput.h
   abstractvideooutput.h
   backendcapabilities.h
   phonondefs.h
   backendinterface.h
   mediaobjectinterface.h
   audiooutputinterface.h
   addoninterface.h
   mediacontroller.h
   videowidget.h
   videowidgetinterface.h
   videoplayer.h
   seekslider.h
   volumeslider.h
   effectwidget.h
   DESTINATION ${INCLUDE_INSTALL_DIR}/phonon COMPONENT Devel)

install( FILES  phononbackend.desktop           DESTINATION  ${SERVICETYPES_INSTALL_DIR} )
install( FILES  org.kde.Phonon.AudioOutput.xml  DESTINATION  ${DBUS_INTERFACES_INSTALL_DIR} )
