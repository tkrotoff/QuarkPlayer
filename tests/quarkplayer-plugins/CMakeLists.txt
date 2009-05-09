project(tests)

set(tests_SRCS
	main.cpp
	filebrowser/FileSearchModelTest.cpp
	playlist/PlaylistModelTest.cpp
)

qt4_wrap_cpp(tests_SRCS
	filebrowser/FileSearchModelTest.h
	playlist/PlaylistModelTest.h
)

add_executable(tests ${tests_SRCS})

target_link_libraries(tests
	filebrowser
	playlist
	modeltest
	${QT_QTTEST_LIBRARY}
)

install(TARGETS tests DESTINATION ${BIN_INSTALL_DIR}/tests)
if (WIN32)
	install(PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/tests.bat DESTINATION ${BIN_INSTALL_DIR}/tests)
endif (WIN32)
