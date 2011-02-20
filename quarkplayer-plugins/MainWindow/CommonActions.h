/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2011  Tanguy Krotoff <tkrotoff@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef COMMONACTIONS_H
#define COMMONACTIONS_H

#include <quarkplayer-plugins/MainWindow/MainWindowExport.h>

#include <QtCore/QObject>

#include <phonon/phononnamespace.h>

class QuarkPlayer;

namespace Phonon {
	class MediaObject;
	class AudioOutput;
}

/**
 * Common QActions.
 *
 * @author Tanguy Krotoff
 */
class MAINWINDOW_API CommonActions : public QObject {
	Q_OBJECT
public:

	CommonActions(QuarkPlayer & quarkPlayer, QObject * parent);

	~CommonActions();

private slots:

	void stateChanged(Phonon::State newState);

	void retranslate();

	void currentMediaObjectChanged(Phonon::MediaObject * mediaObject);

private:

	void populateActionCollection();

	QuarkPlayer & _quarkPlayer;
};

#endif	//COMMONACTIONS_H
