#
# This Makefile is used by targets deb and rpm.
#
# Targets deb and rpm create respectively Debian and RPM packages
# for QuarkPlayer. This Makefile cannot be used without CMake.
# Target package from CMake is not powerful enough cf
# http://article.gmane.org/gmane.comp.programming.tools.cmake.user/16242
# See:
# packaging/rpm/CMakeLists.txt
# packaging/rpm/quarkplayer.spec.in
# packaging/debian/CMakeLists.txt
# packaging/debian/rules.in
# packaging/debian/control.in
#

# Can be changed to /usr/local for example
PREFIX = /usr

# Output directory of CMake install target
#BUILD_DIR = build/linux-gcc43-release

# Source code directory
#CMAKE_SOURCE_DIR = .

# Directory where to run make
#CMAKE_BINARY_DIR = build

# Output directory for installing binaries
#DESTDIR = build/debian/quarkplayer

all: quarkplayer-src

configure:
	cd $(CMAKE_BINARY_DIR) && ./build_make-release.sh

# Cannot name this target 'quarkplayer' since a directory has this name already
quarkplayer-src:
	cd $(CMAKE_BINARY_DIR) && $(MAKE) install

clean:
	cd $(CMAKE_BINARY_DIR) && $(MAKE) clean

install:

	# Binary
	install -d $(DESTDIR)$(PREFIX)/bin
	install -m 755 $(BUILD_DIR)/quarkplayer $(DESTDIR)$(PREFIX)/bin

	# Internal libraries
	install -d $(DESTDIR)$(PREFIX)/lib/quarkplayer
	install -m 644 $(BUILD_DIR)/*.so* $(DESTDIR)$(PREFIX)/lib/quarkplayer

	# Plugins
	install -d $(DESTDIR)$(PREFIX)/lib/quarkplayer/plugins
	install -m 644 $(BUILD_DIR)/plugins/*.so* $(DESTDIR)$(PREFIX)/lib/quarkplayer/plugins

	# MPlayer Phonon backend
	install -d $(DESTDIR)$(PREFIX)/lib/quarkplayer/phonon_backend
	install -m 644 $(BUILD_DIR)/phonon_backend/*.so* $(DESTDIR)$(PREFIX)/lib/quarkplayer/phonon_backend

	# Qt styles
	install -d $(DESTDIR)$(PREFIX)/lib/quarkplayer/styles
	install -m 644 $(BUILD_DIR)/styles/*.so* $(DESTDIR)$(PREFIX)/lib/quarkplayer/styles

	# Translations
	install -d $(DESTDIR)$(PREFIX)/share/quarkplayer/translations
	install -m 644 $(BUILD_DIR)/translations/*.qm $(DESTDIR)$(PREFIX)/share/quarkplayer/translations

	# Man pages
	install -d $(DESTDIR)$(PREFIX)/share/man/man1

	# .desktop file
	install -d $(DESTDIR)$(PREFIX)/share/applications
	install -m 644 $(CMAKE_SOURCE_DIR)/quarkplayer-app/quarkplayer.desktop $(DESTDIR)$(PREFIX)/share/applications

	# KDE Icons
	install -d $(DESTDIR)$(PREFIX)/share/icons/hicolor/16x16/apps
	install -d $(DESTDIR)$(PREFIX)/share/icons/hicolor/22x22/apps
	install -d $(DESTDIR)$(PREFIX)/share/icons/hicolor/32x32/apps
	install -d $(DESTDIR)$(PREFIX)/share/icons/hicolor/48x48/apps
	install -d $(DESTDIR)$(PREFIX)/share/icons/hicolor/64x64/apps
	install -d $(DESTDIR)$(PREFIX)/share/icons/hicolor/128x128/apps
	install -d $(DESTDIR)$(PREFIX)/share/icons/hicolor/scalable/apps
	install -m 644 $(CMAKE_SOURCE_DIR)/quarkplayer-app/icons/quarkplayer-16x16.png $(DESTDIR)$(PREFIX)/share/icons/hicolor/16x16/apps/quarkplayer.png
	install -m 644 $(CMAKE_SOURCE_DIR)/quarkplayer-app/icons/quarkplayer-22x22.png $(DESTDIR)$(PREFIX)/share/icons/hicolor/22x22/apps/quarkplayer.png
	install -m 644 $(CMAKE_SOURCE_DIR)/quarkplayer-app/icons/quarkplayer-32x32.png $(DESTDIR)$(PREFIX)/share/icons/hicolor/32x32/apps/quarkplayer.png
	install -m 644 $(CMAKE_SOURCE_DIR)/quarkplayer-app/icons/quarkplayer-48x48.png $(DESTDIR)$(PREFIX)/share/icons/hicolor/48x48/apps/quarkplayer.png
	install -m 644 $(CMAKE_SOURCE_DIR)/quarkplayer-app/icons/quarkplayer-64x64.png $(DESTDIR)$(PREFIX)/share/icons/hicolor/64x64/apps/quarkplayer.png
	install -m 644 $(CMAKE_SOURCE_DIR)/quarkplayer-app/icons/quarkplayer.svg $(DESTDIR)$(PREFIX)/share/icons/hicolor/scalable/apps/quarkplayer.svg

uninstall:

	# Binary
	rm -f $(DESTDIR)$(PREFIX)/bin/quarkplayer

	# Internal libraries
	rm -f $(DESTDIR)$(PREFIX)/lib/quarkplayer/*.so*

	# Plugins
	rm -f $(DESTDIR)$(PREFIX)/lib/quarkplayer/plugins/*.so*

	# MPlayer Phonon backend
	rm -f $(DESTDIR)$(PREFIX)/lib/quarkplayer/phonon_backend/*.so*

	# Qt styles
	rm -f $(DESTDIR)$(PREFIX)/lib/quarkplayer/styles/*.so*

	# Translations
	rm -f $(DESTDIR)$(PREFIX)/share/quarkplayer/translations/*.qm

	# Man pages
	rm -f $(DESTDIR)$(PREFIX)/share/man/man1/quarkplayer.1.gz

	# .desktop file
	rm -f $(DESTDIR)$(PREFIX)/share/applications/quarkplayer.desktop

	# KDE Icons
	rm -f $(DESTDIR)$(PREFIX)/share/icons/hicolor/16x16/apps/quarkplayer.png
	rm -f $(DESTDIR)$(PREFIX)/share/icons/hicolor/22x22/apps/quarkplayer.png
	rm -f $(DESTDIR)$(PREFIX)/share/icons/hicolor/32x32/apps/quarkplayer.png
	rm -f $(DESTDIR)$(PREFIX)/share/icons/hicolor/48x48/apps/quarkplayer.png
	rm -f $(DESTDIR)$(PREFIX)/share/icons/hicolor/64x64/apps/quarkplayer.png
	rm -f $(DESTDIR)$(PREFIX)/share/icons/hicolor/128x128/apps/quarkplayer.png
	rm -f $(DESTDIR)$(PREFIX)/share/icons/hicolor/scalable/apps/quarkplayer.svg
