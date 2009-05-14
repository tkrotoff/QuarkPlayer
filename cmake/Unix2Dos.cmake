# - unix2dos(src dst)
# Converts a UNIX ASCII file to a DOS/Windows ASCII file, i.e change from LF to CR+LF end of line character.
#
# This only works under Windows, rational: when coding your files all use CR+LF on your repository.
# This macro is usefull when you want to convert files like "README", "ChangeLog", "COPYING"...
# in order to read them using Notepad under Windows.
#
# See http://en.wikipedia.org/wiki/Newline#Conversion_utilities
#
# Internal DOS command used:
# TYPE unix_file | FIND "" /V > dos_file
#
# Example:
# unix2dos(unix_file dos_file)
#
# Copyright (C) 2009  Tanguy Krotoff <tkrotoff@gmail.com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING file.


macro (unix2dos unix_file dos_file)

	file(TO_NATIVE_PATH ${unix_file} unix_file_native)
	file(TO_NATIVE_PATH ${dos_file} dos_file_native)

	add_custom_command (
		OUTPUT ${dos_file}
		COMMAND type \"${unix_file_native}\"
		COMMENT "kikoo"
	)

	#execute_process(
	#	COMMAND type \"${unix_file_native}\"
	#	COMMAND %SystemRoot%\\system32\\find \"\" /V > \"${dos_file_native}\"
	#	RESULT_VARIABLE result
	#	OUTPUT_VARIABLE output
	#	ERROR_VARIABLE error
	#)
	#message("result=${result}")
	#message("output=${output}")
	#message("error=${error}")

endmacro (unix2dos)
