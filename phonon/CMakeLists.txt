project(phonon)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(PHONON_LIB_MAJOR_VERSION "4")
set(PHONON_LIB_MINOR_VERSION "2")
set(PHONON_LIB_PATCH_VERSION "70")
set(PHONON_LIB_VERSION "${PHONON_LIB_MAJOR_VERSION}.3.0")
set(PHONON_LIB_SOVERSION ${PHONON_LIB_MAJOR_VERSION})

set(SHARE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}/share")
set(KDE4_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}")
set(DBUS_INTERFACES_INSTALL_DIR "${KDE4_INSTALL_DIR}/share/dbus-1/interfaces" )

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
   list(APPEND phonon_LIB_SRCS
      audiooutputadaptor.cpp
   )
endif (QT_QTDBUS_FOUND)

qt4_automoc(${phonon_LIB_SRCS})
add_library(phonon SHARED ${phonon_LIB_SRCS})
if (QT_QTDBUS_FOUND)
   target_link_libraries(phonon ${QT_QTDBUS_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})
else (QT_QTDBUS_FOUND)
   target_link_libraries(phonon ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})
endif (QT_QTDBUS_FOUND)

set_target_properties(phonon PROPERTIES
                             VERSION ${PHONON_LIB_VERSION}
                             SOVERSION ${PHONON_LIB_SOVERSION}
                             DEFINE_SYMBOL MAKE_PHONON_LIB
                             )
install(TARGETS phonon ${INSTALL_TARGETS_DEFAULT_ARGS})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/phononnamespace.h.in ${CMAKE_CURRENT_BINARY_DIR}/phononnamespace.h)
