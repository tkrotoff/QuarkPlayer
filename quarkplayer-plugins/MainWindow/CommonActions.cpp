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

#include <TkUtil/ActionCollection.h>
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

	Actions.add("Common.OpenFile", new TkAction(app, QKeySequence::Open));
	Actions.add("Common.Quit", new TkAction(app, tr("Ctrl+Q"), tr("Alt+X")));
	Actions.add("Common.ReportBug", new QAction(app));
	Actions.add("Common.ShowMailingList", new QAction(app));
	Actions.add("Common.ShowLog", new QAction(app));
	Actions.add("Common.About", new TkAction(app, tr("Ctrl+F1")));
	Actions.add("Common.AboutQt", new QAction(app));
	Actions.add("Common.OpenDVD", new TkAction(app, tr("Ctrl+D")));
	Actions.add("Common.OpenURL", new TkAction(app, tr("Ctrl+U")));
	Actions.add("Common.OpenVCD", new QAction(app));
	Actions.add("Common.NewMediaObject", new QAction(app));
	Actions.add("Common.Equalizer", new TkAction(app, tr("Ctrl+E")));
	Actions.add("Common.Configure", new QAction(app));
	Actions.add("Common.EmptyMenu", new QAction(app));

	TkAction * action = new TkAction(app, tr("Space"), Qt::Key_MediaPlay, Qt::Key_Pause);
	action->setShortcutContext(Qt::ApplicationShortcut);
	Actions.add("Common.PlayPause", action);
	action = new TkAction(app, Qt::Key_MediaStop);
	Actions.add("Common.Stop", action);
	action = new TkAction(app, tr("Ctrl+N"), tr(">"), Qt::Key_MediaNext);
	action->setShortcutContext(Qt::ApplicationShortcut);
	Actions.add("Common.NextTrack", action);
	action = new TkAction(app, tr("Ctrl+P"), tr("<"), Qt::Key_MediaPrevious);
	action->setShortcutContext(Qt::ApplicationShortcut);
	Actions.add("Common.PreviousTrack", action);

	action = new TkAction(app, tr("Left"));
	action->setShortcutContext(Qt::ApplicationShortcut);
	Actions.add("Common.JumpBackward10s", action);
	action = new TkAction(app, tr("Ctrl+Left"));
	action->setShortcutContext(Qt::ApplicationShortcut);
	Actions.add("Common.JumpBackward1min", action);
	action = new TkAction(app, tr("Shift+Left"));
	action->setShortcutContext(Qt::ApplicationShortcut);
	Actions.add("Common.JumpBackward10min", action);

	action = new TkAction(app, tr("Right"));
	action->setShortcutContext(Qt::ApplicationShortcut);
	Actions.add("Common.JumpForward10s", action);
	action = new TkAction(app, tr("Ctrl+Right"));
	action->setShortcutContext(Qt::ApplicationShortcut);
	Actions.add("Common.JumpForward1min", action);
	action = new TkAction(app, tr("Shift+Right"));
	action->setShortcutContext(Qt::ApplicationShortcut);
	Actions.add("Common.JumpForward10min", action);

	action = new TkAction(app, tr("["));
	action->setShortcutContext(Qt::ApplicationShortcut);
	Actions.add("Common.SpeedDecrease10%", action);
	action = new TkAction(app, tr("]"));
	action->setShortcutContext(Qt::ApplicationShortcut);
	Actions.add("Common.SpeedIncrease10%", action);

	action = new TkAction(app, tr("Ctrl+M"));
	action->setShortcutContext(Qt::ApplicationShortcut);
	action->setCheckable(true);
	Actions.add("Common.VolumeMute", action);

	action = new TkAction(app, tr("Ctrl+Down"), tr("-"), tr("Alt+-"));
	action->setShortcutContext(Qt::ApplicationShortcut);
	Actions.add("Common.VolumeDecrease10%", action);
	action = new TkAction(app, tr("Ctrl+Up"), tr("+"), tr("Alt++"));
	action->setShortcutContext(Qt::ApplicationShortcut);
	Actions.add("Common.VolumeIncrease10%", action);

	action = new TkAction(app, tr("Ctrl+F"), tr("Alt+Return"));
	action->setShortcutContext(Qt::ApplicationShortcut);
	action->setCheckable(true);
	Actions.add("Common.FullScreen", action);

	action = new TkAction(app, tr("Esc"), tr("Backspace"));
	Actions.add("Common.FullScreenExit", action);
}

void CommonActions::retranslate() {
	Actions["Common.OpenFile"]->setText(tr("Play &File..."));
	Actions["Common.OpenFile"]->setIcon(QIcon::fromTheme("document-open"));

	Actions["Common.Quit"]->setText(tr("&Quit"));
	Actions["Common.Quit"]->setIcon(QIcon::fromTheme("application-exit"));

	Actions["Common.ReportBug"]->setText(tr("&Report a Problem..."));
	if (desktopEnvironment() == GNOME) {
		Actions["Common.ReportBug"]->setIcon(QIcon::fromTheme("apport"));
	} else {
		Actions["Common.ReportBug"]->setIcon(QIcon::fromTheme("tools-report-bug"));
	}

	Actions["Common.ShowMailingList"]->setText(tr("&Discuss about QuarkPlayer..."));
	if (desktopEnvironment() == GNOME) {
		Actions["Common.ShowMailingList"]->setIcon(QIcon::fromTheme("help-faq"));
	} else {
		Actions["Common.ShowMailingList"]->setIcon(QIcon::fromTheme("help-hint"));
	}

	Actions["Common.ShowLog"]->setText(tr("View &Log"));
	QIcon logIcon;
	if (desktopEnvironment() == GNOME) {
		logIcon = QIcon::fromTheme("logviewer");
	} else {
		logIcon = QIcon::fromTheme("text-x-log");
	}
	Actions["Common.ShowLog"]->setIcon(logIcon);

	Actions["Common.About"]->setText(tr("&About"));

	Actions["Common.AboutQt"]->setText(tr("About &Qt"));

	Actions["Common.OpenDVD"]->setText(tr("Play &DVD..."));
	Actions["Common.OpenDVD"]->setIcon(QIcon::fromTheme("media-optical"));

	Actions["Common.OpenURL"]->setText(tr("Play &URL..."));

	Actions["Common.OpenVCD"]->setText(tr("Play &VCD..."));
\
	Actions["Common.NewMediaObject"]->setText(tr("New Media Window"));
	Actions["Common.NewMediaObject"]->setIcon(QIcon::fromTheme("tab-new"));

	Actions["Common.Equalizer"]->setText(tr("&Equalizer..."));
	Actions["Common.Equalizer"]->setIcon(QIcon::fromTheme("view-media-equalizer"));

	Actions["Common.Configure"]->setText(tr("&Configure..."));
	Actions["Common.Configure"]->setIcon(QIcon::fromTheme("preferences-system"));

	Actions["Common.EmptyMenu"]->setText(tr("<empty>"));
	Actions["Common.EmptyMenu"]->setEnabled(false);

	Actions["Common.PreviousTrack"]->setText(tr("P&revious Track"));
	Actions["Common.PreviousTrack"]->setIcon(QIcon::fromTheme("media-skip-backward"));

	Actions["Common.PlayPause"]->setText(tr("&Play/Pause"));
	Actions["Common.PlayPause"]->setIcon(QIcon::fromTheme("media-playback-start"));

	Actions["Common.Stop"]->setText(tr("&Stop"));
	Actions["Common.Stop"]->setIcon(QIcon::fromTheme("media-playback-stop"));

	Actions["Common.NextTrack"]->setText(tr("&Next Track"));
	Actions["Common.NextTrack"]->setIcon(QIcon::fromTheme("media-skip-forward"));

	Actions["Common.JumpBackward10s"]->setText(tr("Jump &Backward 10s"));
	Actions["Common.JumpBackward1min"]->setText(tr("Jump &Backward 1min"));
	Actions["Common.JumpBackward1min"]->setIcon(QIcon::fromTheme("media-seek-backward"));
	Actions["Common.JumpBackward10min"]->setText(tr("Jump &Backward 10min"));
	Actions["Common.JumpForward10s"]->setText(tr("Jump &Forward 10s"));
	Actions["Common.JumpForward1min"]->setText(tr("Jump &Forward 1min"));
	Actions["Common.JumpForward1min"]->setIcon(QIcon::fromTheme("media-seek-forward"));
	Actions["Common.JumpForward10min"]->setText(tr("Jump &Forward 10min"));
	Actions["Common.SpeedDecrease10%"]->setText(tr("Decrease Speed"));
	Actions["Common.SpeedIncrease10%"]->setText(tr("Increase Speed"));

	Actions["Common.VolumeMute"]->setText(tr("&Mute"));
	Actions["Common.VolumeMute"]->setIcon(QIcon::fromTheme("audio-volume-muted"));
	Actions["Common.VolumeDecrease10%"]->setText(tr("&Decrease Volume"));
	Actions["Common.VolumeIncrease10%"]->setText(tr("&Increase Volume"));

	Actions["Common.FullScreen"]->setText(tr("&Fullscreen"));
	Actions["Common.FullScreen"]->setIcon(QIcon::fromTheme("view-fullscreen"));

	Actions["Common.FullScreenExit"]->setText(tr("&Exit Fullscreen"));
}

void CommonActions::stateChanged(Phonon::State newState) {
	//Enabled/disabled fullscreen button depending if media is a video or audio
	if (_quarkPlayer.currentMediaObject()->hasVideo()) {
		Actions["Common.FullScreen"]->setEnabled(true);
	} else {
		Actions["Common.FullScreen"]->setEnabled(false);
	}

	switch (newState) {
	case Phonon::ErrorState:
		break;

	case Phonon::PlayingState:
		Actions["Common.PlayPause"]->setText(tr("&Pause"));
		Actions["Common.PlayPause"]->setIcon(QIcon::fromTheme("media-playback-pause"));
		disconnect(Actions["Common.PlayPause"], 0, 0, 0);
		connect(Actions["Common.PlayPause"], SIGNAL(triggered()),
			_quarkPlayer.currentMediaObject(), SLOT(pause()));

		Actions["Common.Stop"]->setEnabled(true);
		break;

	case Phonon::StoppedState:
		Actions["Common.PlayPause"]->setText(tr("P&lay"));
		Actions["Common.PlayPause"]->setIcon(QIcon::fromTheme("media-playback-start"));
		disconnect(Actions["Common.PlayPause"], 0, 0, 0);
		connect(Actions["Common.PlayPause"], SIGNAL(triggered()),
			_quarkPlayer.currentMediaObject(), SLOT(play()));

		Actions["Common.Stop"]->setEnabled(false);
		break;

	case Phonon::PausedState:
		Actions["Common.PlayPause"]->setText(tr("P&lay"));
		Actions["Common.PlayPause"]->setIcon(QIcon::fromTheme("media-playback-start"));
		disconnect(Actions["Common.PlayPause"], 0, 0, 0);
		connect(Actions["Common.PlayPause"], SIGNAL(triggered()),
			_quarkPlayer.currentMediaObject(), SLOT(play()));

		Actions["Common.Stop"]->setEnabled(true);
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
	disconnect(Actions["Common.Stop"], 0, 0, 0);
	connect(Actions["Common.Stop"], SIGNAL(triggered()),
		mediaObject, SLOT(stop()));
}
