# Shows general debug informations (OS, processor, compiler, build type...)
#
# Copyright (C) 2006-2007  Wengo
# Copyright (C) 2007-2008  Tanguy Krotoff <tkrotoff@gmail.com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING file.


message(STATUS "OS: " ${CMAKE_SYSTEM})
message(STATUS "Processor: " ${CMAKE_SYSTEM_PROCESSOR})
message(STATUS "Compiler: " ${CMAKE_C_COMPILER})
message(STATUS "Build type: " ${CMAKE_BUILD_TYPE})
message(STATUS "Build tool: " ${CMAKE_BUILD_TOOL})
message(STATUS "Build directory: " ${BUILD_DIR})
message(STATUS "SVN revision: " ${SVN_REVISION})
