/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2009  Tanguy Krotoff <tkrotoff@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef QUICKSETTINGSWINDOW_H
#define QUICKSETTINGSWINDOW_H

#include <quarkplayer/PluginInterface.h>

#include <phonon/path.h>

#include <QtGui/QDialog>

#include <QtCore/QHash>

namespace Ui { class QuickSettingsWindow; }

class QuarkPlayer;

namespace Phonon {
	class MediaObject;
	class AudioOutput;
	class VideoWidget;
	class Effect;
}

/**
 * Quick settings window.
 *
 * Brightness, hue, saturation, scale mode, aspect, effects...
 *
 * @author Tanguy Krotoff
 */
class QuickSettingsWindow : public QDialog, public PluginInterface {
	Q_OBJECT
public:

	QuickSettingsWindow(QuarkPlayer & quarkPlayer, const QUuid & uuid);

	~QuickSettingsWindow();

	QString name() const { return tr("Quick settings"); }
	QString description() const { return tr("Window to configure the backend (equalizer, effects...)"); }
	QString version() const { return "0.0.1"; }
	QString webpage() const { return "http://quarkplayer.googlecode.com/"; }
	QString email() const { return "quarkplayer@googlegroups.com"; }
	QString authors() const { return "Tanguy Krotoff"; }
	QString license() const { return "GNU GPLv3+"; }
	QString copyright() const { return "Copyright (C) Tanguy Krotoff"; }

private slots:

	void show();

	void updateEffect();

	void effectChanged();

	void configureEffect();

	void setAspect(int);
	void setScale(int);
	void setSaturation(int);
	void setContrast(int);
	void setHue(int);
	void setBrightness(int);

	void saveSettings();

	void restoreSettings();

	void retranslate();

private:

	void init();

	Ui::QuickSettingsWindow * _ui;

	Phonon::Effect * _nextEffect;

	QHash<QString, QWidget *> _propertyControllers;

	Phonon::AudioOutput * _audioOutput;

	Phonon::Path _audioOutputPath;

	Phonon::MediaObject * _mediaObject;

	Phonon::VideoWidget * _videoWidget;
};

#include <quarkplayer/PluginFactory.h>

class QuickSettingsWindowFactory : public QObject, public PluginFactory {
	Q_OBJECT
	Q_INTERFACES(PluginFactory)
public:

	QString pluginName() const;

	QStringList dependencies() const;

	PluginInterface * create(QuarkPlayer & quarkPlayer, const QUuid & uuid) const;
};

#endif	//QUICKSETTINGSWINDOW_H
