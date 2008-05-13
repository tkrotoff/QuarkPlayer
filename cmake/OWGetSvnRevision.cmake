# - ow_get_svn_revision(revision)
# Gets current subversion revision number
#
# Performs a svnversion command line on ${CMAKE_SOURCE_DIR} directory
# You need to install the subversion command line, check http://subversion.tigris.org/
# if you don't have it already
# ow_get_svn_revision() is being called by OWInitializationInfo.cmake that sets
# SVN_REVSION variable already thus you don't need to call ow_get_svn_revision()
#
# Example:
# set(SVN_REVISION "0")
# ow_get_svn_revision(SVN_REVISION)
# message(STATUS "svn revision: " ${SVN_REVISION})
#
# Copyright (C) 2006  Andreas Schneider <mail@cynapses.org>
# Copyright (C) 2006-2007  Wengo
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING file.


macro (ow_get_svn_revision revision)

	if (NOT Subversion_FOUND)
		find_package(Subversion)
	endif (NOT Subversion_FOUND)

	message(Subversion_SVN_EXECUTABLE=${Subversion_SVN_EXECUTABLE})

    EXECUTE_PROCESS(COMMAND ${Subversion_SVN_EXECUTABLE} --version
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      OUTPUT_VARIABLE Subversion_VERSION_SVN
      OUTPUT_STRIP_TRAILING_WHITESPACE)

message(Subversion_VERSION_SVN=${Subversion_VERSION_SVN})

    EXECUTE_PROCESS(COMMAND ${Subversion_SVN_EXECUTABLE} info ${CMAKE_SOURCE_DIR}
      OUTPUT_VARIABLE ${revision}
      ERROR_VARIABLE Subversion_svn_info_error
      RESULT_VARIABLE Subversion_svn_info_result
      OUTPUT_STRIP_TRAILING_WHITESPACE)

message("Current revision is ${${revision}}")

    EXECUTE_PROCESS(COMMAND
      ${Subversion_SVN_EXECUTABLE} log -r BASE ${CMAKE_SOURCE_DIR}
      OUTPUT_VARIABLE Subversion_LAST_CHANGED_LOG
      ERROR_VARIABLE Subversion_svn_log_error
      RESULT_VARIABLE Subversion_svn_log_result
      OUTPUT_STRIP_TRAILING_WHITESPACE)

message("Subversion_LAST_CHANGED_LOG=${Subversion_LAST_CHANGED_LOG}")


	if (Subversion_FOUND)
		Subversion_WC_INFO("${CMAKE_SOURCE_DIR}" Project)
		message("Current revision is ${Project_WC_REVISION} ${CMAKE_SOURCE_DIR} ${PROJECT_SOURCE_DIR}")
		set(${revision} ${Project_WC_REVISION})
	else (Subversion_FOUND)
		message("Subversion (svn) command line not found, it is recommended to install it")
		set(${revision} "")
	endif (Subversion_FOUND)

endmacro (ow_get_svn_revision)
