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

#include "CommonActions.h"

#include "MainWindowLogger.h"

#include <quarkplayer/QuarkPlayer.h>

#include <TkUtil/Actions.h>
#include <TkUtil/TkAction.h>
#include <TkUtil/DesktopEnvironment.h>
#include <TkUtil/LanguageChangeEventFilter.h>

#include <phonon/mediaobject.h>
#include <phonon/audiooutput.h>

#include <QtGui/QtGui>

CommonActions::CommonActions(QuarkPlayer & quarkPlayer, QObject * parent)
	: QObject(parent),
	_quarkPlayer(quarkPlayer) {

	populateActionCollection();

	if (quarkPlayer.currentMediaObject()) {
		//The current MediaObject has been already created
		//So to wait for the signal is useless: it was already sent long before
		currentMediaObjectChanged(quarkPlayer.currentMediaObject());
	}
	connect(&quarkPlayer, SIGNAL(currentMediaObjectChanged(Phonon::MediaObject *)),
		SLOT(currentMediaObjectChanged(Phonon::MediaObject *)));

	RETRANSLATE(this);
	retranslate();
}

CommonActions::~CommonActions() {
}

void CommonActions::populateActionCollection() {
	QCoreApplication * app = QApplication::instance();
	Q_ASSERT(app);

	Actions.add("Global.OpenFile", new TkAction(app, QKeySequence::Open));
	Actions.add("Global.Quit", new TkAction(app, tr("Ctrl+Q"), tr("Alt+X")));
	Actions.add("Global.ReportBug", new QAction(app));
	Actions.add("Global.ShowMailingList", new QAction(app));
	Actions.add("Global.ShowLog", new QAction(app));
	Actions.add("Global.About", new TkAction(app, tr("Ctrl+F1")));
	Actions.add("Global.AboutQt", new QAction(app));
	Actions.add("Global.OpenDVD", new TkAction(app, tr("Ctrl+D")));
	Actions.add("Global.OpenURL", new TkAction(app, tr("Ctrl+U")));
	Actions.add("Global.OpenVCD", new QAction(app));
	Actions.add("Global.NewMediaObject", new QAction(app));
	Actions.add("Global.Equalizer", new TkAction(app, tr("Ctrl+E")));
	Actions.add("Global.Configure", new QAction(app));
	Actions.add("Global.EmptyMenu", new QAction(app));

	TkAction * action = new TkAction(app, tr("Space"), Qt::Key_MediaPlay, Qt::Key_Pause);
	action->setShortcutContext(Qt::ApplicationShortcut);
	Actions.add("Global.PlayPause", action);
	action = new TkAction(app, Qt::Key_MediaStop);
	Actions.add("Global.Stop", action);
	action = new TkAction(app, tr("Ctrl+N"), tr(">"), Qt::Key_MediaNext);
	action->setShortcutContext(Qt::ApplicationShortcut);
	Actions.add("Global.NextTrack", action);
	action = new TkAction(app, tr("Ctrl+P"), tr("<"), Qt::Key_MediaPrevious);
	action->setShortcutContext(Qt::ApplicationShortcut);
	Actions.add("Global.PreviousTrack", action);

	action = new TkAction(app, tr("Left"));
	action->setShortcutContext(Qt::ApplicationShortcut);
	Actions.add("Global.JumpBackward10s", action);
	action = new TkAction(app, tr("Ctrl+Left"));
	action->setShortcutContext(Qt::ApplicationShortcut);
	Actions.add("Global.JumpBackward1min", action);
	action = new TkAction(app, tr("Shift+Left"));
	action->setShortcutContext(Qt::ApplicationShortcut);
	Actions.add("Global.JumpBackward10min", action);

	action = new TkAction(app, tr("Right"));
	action->setShortcutContext(Qt::ApplicationShortcut);
	Actions.add("Global.JumpForward10s", action);
	action = new TkAction(app, tr("Ctrl+Right"));
	action->setShortcutContext(Qt::ApplicationShortcut);
	Actions.add("Global.JumpForward1min", action);
	action = new TkAction(app, tr("Shift+Right"));
	action->setShortcutContext(Qt::ApplicationShortcut);
	Actions.add("Global.JumpForward10min", action);

	action = new TkAction(app, tr("["));
	action->setShortcutContext(Qt::ApplicationShortcut);
	Actions.add("Global.SpeedDecrease10%", action);
	action = new TkAction(app, tr("]"));
	action->setShortcutContext(Qt::ApplicationShortcut);
	Actions.add("Global.SpeedIncrease10%", action);

	action = new TkAction(app, tr("Ctrl+M"));
	action->setShortcutContext(Qt::ApplicationShortcut);
	action->setCheckable(true);
	Actions.add("Global.VolumeMute", action);

	action = new TkAction(app, tr("Ctrl+Down"), tr("-"), tr("Alt+-"));
	action->setShortcutContext(Qt::ApplicationShortcut);
	Actions.add("Global.VolumeDecrease10%", action);
	action = new TkAction(app, tr("Ctrl+Up"), tr("+"), tr("Alt++"));
	action->setShortcutContext(Qt::ApplicationShortcut);
	Actions.add("Global.VolumeIncrease10%", action);

	action = new TkAction(app, tr("Ctrl+F"), tr("Alt+Return"));
	action->setShortcutContext(Qt::ApplicationShortcut);
	action->setCheckable(true);
	Actions.add("Global.FullScreen", action);

	action = new TkAction(app, tr("Esc"));
	Actions.add("Global.FullScreenExit", action);
}

void CommonActions::retranslate() {
	Actions["Global.OpenFile"]->setText(tr("Play &File..."));
	Actions["Global.OpenFile"]->setIcon(QIcon::fromTheme("document-open"));

	Actions["Global.Quit"]->setText(tr("&Quit"));
	Actions["Global.Quit"]->setIcon(QIcon::fromTheme("application-exit"));

	Actions["Global.ReportBug"]->setText(tr("&Report a Problem..."));
	if (desktopEnvironment() == GNOME) {
		Actions["Global.ReportBug"]->setIcon(QIcon::fromTheme("apport"));
	} else {
		Actions["Global.ReportBug"]->setIcon(QIcon::fromTheme("tools-report-bug"));
	}

	Actions["Global.ShowMailingList"]->setText(tr("&Discuss about QuarkPlayer..."));
	if (desktopEnvironment() == GNOME) {
		Actions["Global.ShowMailingList"]->setIcon(QIcon::fromTheme("help-faq"));
	} else {
		Actions["Global.ShowMailingList"]->setIcon(QIcon::fromTheme("help-hint"));
	}

	Actions["Global.ShowLog"]->setText(tr("View &Log"));
	QIcon logIcon;
	if (desktopEnvironment() == GNOME) {
		logIcon = QIcon::fromTheme("logviewer");
	} else {
		logIcon = QIcon::fromTheme("text-x-log");
	}
	Actions["Global.ShowLog"]->setIcon(logIcon);

	Actions["Global.About"]->setText(tr("&About"));

	Actions["Global.AboutQt"]->setText(tr("About &Qt"));

	Actions["Global.OpenDVD"]->setText(tr("Play &DVD..."));
	Actions["Global.OpenDVD"]->setIcon(QIcon::fromTheme("media-optical"));

	Actions["Global.OpenURL"]->setText(tr("Play &URL..."));

	Actions["Global.OpenVCD"]->setText(tr("Play &VCD..."));
\
	Actions["Global.NewMediaObject"]->setText(tr("New Media Window"));
	Actions["Global.NewMediaObject"]->setIcon(QIcon::fromTheme("tab-new"));

	Actions["Global.Equalizer"]->setText(tr("&Equalizer..."));
	Actions["Global.Equalizer"]->setIcon(QIcon::fromTheme("view-media-equalizer"));

	Actions["Global.Configure"]->setText(tr("&Configure..."));
	Actions["Global.Configure"]->setIcon(QIcon::fromTheme("preferences-system"));

	Actions["Global.EmptyMenu"]->setText(tr("<empty>"));
	Actions["Global.EmptyMenu"]->setEnabled(false);

	Actions["Global.PreviousTrack"]->setText(tr("P&revious Track"));
	Actions["Global.PreviousTrack"]->setIcon(QIcon::fromTheme("media-skip-backward"));

	Actions["Global.PlayPause"]->setText(tr("&Play/Pause"));
	Actions["Global.PlayPause"]->setIcon(QIcon::fromTheme("media-playback-start"));

	Actions["Global.Stop"]->setText(tr("&Stop"));
	Actions["Global.Stop"]->setIcon(QIcon::fromTheme("media-playback-stop"));

	Actions["Global.NextTrack"]->setText(tr("&Next Track"));
	Actions["Global.NextTrack"]->setIcon(QIcon::fromTheme("media-skip-forward"));

	Actions["Global.JumpBackward10s"]->setText(tr("Jump &Backward 10s"));
	Actions["Global.JumpBackward1min"]->setText(tr("Jump &Backward 1min"));
	Actions["Global.JumpBackward1min"]->setIcon(QIcon::fromTheme("media-seek-backward"));
	Actions["Global.JumpBackward10min"]->setText(tr("Jump &Backward 10min"));
	Actions["Global.JumpForward10s"]->setText(tr("Jump &Forward 10s"));
	Actions["Global.JumpForward1min"]->setText(tr("Jump &Forward 1min"));
	Actions["Global.JumpForward1min"]->setIcon(QIcon::fromTheme("media-seek-forward"));
	Actions["Global.JumpForward10min"]->setText(tr("Jump &Forward 10min"));
	Actions["Global.SpeedDecrease10%"]->setText(tr("Decrease Speed"));
	Actions["Global.SpeedIncrease10%"]->setText(tr("Increase Speed"));

	Actions["Global.VolumeMute"]->setText(tr("&Mute"));
	Actions["Global.VolumeMute"]->setIcon(QIcon::fromTheme("audio-volume-muted"));
	Actions["Global.VolumeDecrease10%"]->setText(tr("&Decrease Volume"));
	Actions["Global.VolumeIncrease10%"]->setText(tr("&Increase Volume"));

	Actions["Global.FullScreen"]->setText(tr("&Fullscreen"));
	Actions["Global.FullScreen"]->setIcon(QIcon::fromTheme("view-fullscreen"));

	Actions["Global.FullScreenExit"]->setText(tr("&Exit Fullscreen"));
}

void CommonActions::stateChanged(Phonon::State newState) {
	//Enabled/disabled fullscreen button depending if media is a video or audio
	if (_quarkPlayer.currentMediaObject()->hasVideo()) {
		Actions["Global.FullScreen"]->setEnabled(true);
	} else {
		Actions["Global.FullScreen"]->setEnabled(false);
	}

	switch (newState) {
	case Phonon::ErrorState:
		break;

	case Phonon::PlayingState:
		Actions["Global.PlayPause"]->setText(tr("&Pause"));
		Actions["Global.PlayPause"]->setIcon(QIcon::fromTheme("media-playback-pause"));
		disconnect(Actions["Global.PlayPause"], 0, 0, 0);
		connect(Actions["Global.PlayPause"], SIGNAL(triggered()),
			_quarkPlayer.currentMediaObject(), SLOT(pause()));

		Actions["Global.Stop"]->setEnabled(true);
		break;

	case Phonon::StoppedState:
		Actions["Global.PlayPause"]->setText(tr("P&lay"));
		Actions["Global.PlayPause"]->setIcon(QIcon::fromTheme("media-playback-start"));
		disconnect(Actions["Global.PlayPause"], 0, 0, 0);
		connect(Actions["Global.PlayPause"], SIGNAL(triggered()),
			_quarkPlayer.currentMediaObject(), SLOT(play()));

		Actions["Global.Stop"]->setEnabled(false);
		break;

	case Phonon::PausedState:
		Actions["Global.PlayPause"]->setText(tr("P&lay"));
		Actions["Global.PlayPause"]->setIcon(QIcon::fromTheme("media-playback-start"));
		disconnect(Actions["Global.PlayPause"], 0, 0, 0);
		connect(Actions["Global.PlayPause"], SIGNAL(triggered()),
			_quarkPlayer.currentMediaObject(), SLOT(play()));

		Actions["Global.Stop"]->setEnabled(true);
		break;

	case Phonon::LoadingState:
		break;

	case Phonon::BufferingState:
		break;

	default:
		MainWindowCritical() << "Unknown state:" << newState;
	}
}

void CommonActions::currentMediaObjectChanged(Phonon::MediaObject * mediaObject) {
	foreach (Phonon::MediaObject * tmp, _quarkPlayer.mediaObjectList()) {
		tmp->disconnect(this);
	}

	connect(mediaObject, SIGNAL(stateChanged(Phonon::State, Phonon::State)),
		SLOT(stateChanged(Phonon::State)));

	//Update current MediaObject state
	stateChanged(mediaObject->state());

	//Actions connect
	disconnect(Actions["Global.Stop"], 0, 0, 0);
	connect(Actions["Global.Stop"], SIGNAL(triggered()),
		mediaObject, SLOT(stop()));
}
