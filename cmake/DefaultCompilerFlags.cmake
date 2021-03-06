# Defines default compiler/linker flags
#
# GNU GCC Command Options: http://gcc.gnu.org/onlinedocs/gcc-4.1.1/gcc/Invoking-GCC.html
# GNU GCC Warning Options: http://gcc.gnu.org/onlinedocs/gcc-4.1.1/gcc/Warning-Options.html
# GNU GCC Link Options: http://gcc.gnu.org/onlinedocs/gcc-4.1.1/gcc/Link-Options.html
# Microsoft Visual C++ Compiler Options: http://msdn2.microsoft.com/en-us/library/fwkeyyhe.aspx
# Microsoft Visual C++ Linker Options: http://msdn2.microsoft.com/en-us/library/y0zzbyt4.aspx
#
# Copyright (C) 2006-2007  Wengo
# Copyright (C) 2006-2007  Andreas Schneider <mail@cynapses.org>
# Copyright (C) 2007-2009  Tanguy Krotoff <tkrotoff@gmail.com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING file.

include(DetectSystem)
include(CheckCXXCompilerFlag)

# With -fPIC, CMake-2.6 should fix this, does not seem to be the case :/
# cf http://code.google.com/p/phonon-vlc-mplayer/issues/detail?id=1
if (GCC)
	if (CMAKE_SIZEOF_VOID_P MATCHES "8")
		check_cxx_compiler_flag("-fPIC" WITH_FPIC)
		if (WITH_FPIC)
			set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
		endif (WITH_FPIC)
	endif (CMAKE_SIZEOF_VOID_P MATCHES "8")
endif (GCC)

# Enable warnings
if (MSVC)
	# This shows a lot of warnings from Microsoft source code!
	# So it is unusable :/
	#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /Wall")
	#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Wall")
else (MSVC)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif (MSVC)
##

if (CMAKE_BUILD_TYPE STREQUAL Debug)
	if (APPLE)
		# Use dwarf-2 debugging format: it produces ~10x smaller executables
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -gdwarf-2")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -gdwarf-2")
	endif (APPLE)

	# Defines DEBUG when in debug mode
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDEBUG")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEBUG")
endif (CMAKE_BUILD_TYPE STREQUAL Debug)

# UNICODE support enabled
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DUNICODE")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUNICODE")
if (WIN32)
	# UNICODE support with Visual C++ and MinGW
	# See Visual C++ Unicode Programming Summary
	# http://msdn.microsoft.com/en-us/library/dybsewaf%28VS.100%29.aspx
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_UNICODE")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_UNICODE")
endif (WIN32)
##

if (MSVC)
	if (MSVC_VERSION GREATER 1399)
		# If using Visual C++ 2005 (MSVC80) and greater (MSVC_VERSION=1400)
		# Disable Visual C++ Security Enhancements in the CRT
		# See http://msdn.microsoft.com/en-us/library/8ef0s5kh%28VS.100%29.aspx
		# This is non standard and Microsoft specific
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE")
	endif (MSVC_VERSION GREATER 1399)
endif (MSVC)

#get_gcc_version(version)
#message(STATUS "GCC Version: ${version}")
#message(STATUS "CFLAGS: ${CMAKE_C_FLAGS}")
#message(STATUS "CXXFLAGS: ${CMAKE_CXX_FLAGS}")
