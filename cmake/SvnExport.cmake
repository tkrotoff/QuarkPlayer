# - svn_export(repository dir)
# Executes 'svn export' given a repository URL and destination directory
#
# You need to install the subversion command line, check http://subversion.tigris.org/
# if you don't have it already
#
# Example:
# svn_export(http://phonon-vlc-mplayer.googlecode.com/svn/trunk/ ${CMAKE_BINARY_DIR}/quarkplayer-svn-export)
#
# Copyright (C) 2009  Tanguy Krotoff <tkrotoff@gmail.com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING file.


macro (svn_export repository dir)

	if (NOT Subversion_FOUND)
		find_package(Subversion)
	endif (NOT Subversion_FOUND)

	if (Subversion_FOUND)
		execute_process(
			COMMAND ${Subversion_SVN_EXECUTABLE} export ${repository} ${dir}
		)
	else (Subversion_FOUND)
	endif (Subversion_FOUND)

endmacro (svn_export)
