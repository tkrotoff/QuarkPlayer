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

	if (Subversion_FOUND)
		Subversion_WC_INFO(${CMAKE_SOURCE_DIR} _TMP)
		set(${revision} ${_TMP_WC_REVISION})

		#Subversion_WC_LOG(${CMAKE_SOURCE_DIR} _TMP)
		#message(STATUS LAST_CHANGED_LOG=${_TMP_LAST_CHANGED_LOG})
	else (Subversion_FOUND)
		message("Subversion (svn) command line not found, it is recommended to install it")
		set(${revision} "")
	endif (Subversion_FOUND)

endmacro (ow_get_svn_revision)
