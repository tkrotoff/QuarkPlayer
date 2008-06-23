# - loc_counter(path loc)
# Gets the LOC of a given path.
#
# Example:
# loc_counter(path loc)
# message(${loc})
#
# Output generated:
# 20070323121316
#
# Copyright (C) 2008  Tanguy Krotoff <tkrotoff@gmail.com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING file.


macro (loc_counter path loc recursive)

	if (NOT QT4_FOUND)
		find_package(Qt4 REQUIRED)
	endif (NOT QT4_FOUND)

	try_run(
		runResult
		compileResult
		${CMAKE_BINARY_DIR}
		${CMAKE_SOURCE_DIR}/cmake/loc_counter.cpp
		COMPILE_DEFINITIONS ${QT_DEFINITIONS} -DQT_NO_DEBUG -I${QT_INCLUDE_DIR}
		CMAKE_FLAGS "-DLINK_LIBRARIES=${QT_QTCORE_LIBRARY}"
		COMPILE_OUTPUT_VARIABLE compileOutput
		RUN_OUTPUT_VARIABLE ${loc}
		ARGS
			"\"${path}\""
			${recursive}
	)

	#message("runResult="${runResult})
	#message("compileOutput="${compileOutput})
	#message("compileResult="${compileResult})

endmacro (loc_counter)
