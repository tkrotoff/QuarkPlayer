BUILD_DIR = linux-gcc43-release

PREFIX=/usr/local

configure:
	cd build && ./build_make-release.sh

quarkplayer: configure
	cd build && $(MAKE)

clean:
	cd build && $(MAKE) clean

install: quarkplayer

	#Binary
	install -d $(PREFIX)/bin
	install -m 755 $(BUILD_DIR)/quarkplayer $(PREFIX)/bin

	#Internal libraries
	install -d $(PREFIX)/lib/quarkplayer
	install -m 644 $(BUILD_DIR)/*.so $(PREFIX)/lib/quarkplayer

	#Plugins
	install -d $(PREFIX)/lib/quarkplayer/plugins
	install -m 644 $(BUILD_DIR)/lib/quarkplayer/plugins/*.so $(PREFIX)/lib/quarkplayer/plugins

	#MPlayer Phonon backend
	install -d $(PREFIX)/lib/quarkplayer/phonon_backend
	install -m 644 $(BUILD_DIR)/phonon_backend/*.so $(PREFIX)/lib/quarkplayer/phonon_backend

	#Qt styles
	install -d $(PREFIX)/lib/quarkplayer/styles
	install -m 644 $(BUILD_DIR)/styles/*.so $(PREFIX)/lib/quarkplayer/styles

	#Translations
	install -d $(PREFIX)/share/quarkplayer/translations
	install -m 644 $(BUILD_DIR)/translations/*.qm $(PREFIX)/share/quarkplayer/translations

	#Documentation
	install -d $(PREFIX)/share/quarkplayer/doc
	install -m 644 $(BUILD_DIR)/README $(PREFIX)/share/quarkplayer/doc
	install -m 644 $(BUILD_DIR)/AUTHORS $(PREFIX)/share/quarkplayer/doc
	install -m 644 $(BUILD_DIR)/ChangeLog $(PREFIX)/share/quarkplayer/doc
	install -m 644 $(BUILD_DIR)/COPYING $(PREFIX)/share/quarkplayer/doc
	install -m 644 $(BUILD_DIR)/COPYING.LESSER $(PREFIX)/share/quarkplayer/doc
	install -m 644 $(BUILD_DIR)/THANKS $(PREFIX)/share/quarkplayer/doc

	#.desktop
	install -d $(PREFIX)/share/applications
	install -m 644 $(BUILD_DIR)/quarkplayer.desktop $(PREFIX)/share/applications

uninstall:

	#Binary
	rm -f $(PREFIX)/bin/quarkplayer

	#Internal libraries
	rm -f $(PREFIX)/lib/quarkplayer/*.so

	#Plugins
	rm -f $(PREFIX)/lib/quarkplayer/plugins/*.so

	#MPlayer Phonon backend
	rm -f $(PREFIX)/lib/quarkplayer/phonon_backend/*.so

	#Qt styles
	rm -f $(PREFIX)/lib/quarkplayer/styles/*.so

	#Translations
	rm -f $(PREFIX)/share/quarkplayer/translations/*.qm

	#Documentation
	rm -f $(PREFIX)/share/quarkplayer/doc/*

	#.desktop
	rm -f $(PREFIX)/share/applications/quarkplayer.desktop
