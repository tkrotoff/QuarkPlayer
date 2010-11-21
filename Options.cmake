option(TAGLIB "Build with TagLib support" ON)
option(MEDIAINFOLIB "Build with MediaInfoLib support" ON)

if (UNIX)
	option(USE_SYSTEM_LIBRARIES "Use the system libraries whenever possible" ON)
endif (UNIX)

option(STATICPLUGINS "Build QuarkPlayer plugins in static mode" ON)
option(BUILD_TESTS "Build QuarkPlayer unit tests" ON)
option(WEBKIT "Build WebBrowser library with QtWebKit support" OFF)
option(COVERAGE "Build using TestCocoon coverage tool" OFF)
