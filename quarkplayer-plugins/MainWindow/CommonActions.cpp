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

	Actions.add("CommonActions.OpenFile", new TkAction(app, QKeySequence::Open));
	Actions.add("CommonActions.Quit", new TkAction(app, tr("Ctrl+Q"), tr("Alt+X")));
	Actions.add("CommonActions.ReportBug", new QAction(app));
	Actions.add("CommonActions.ShowMailingList", new QAction(app));
	Actions.add("CommonActions.ShowLog", new QAction(app));
	Actions.add("CommonActions.About", new TkAction(app, tr("Ctrl+F1")));
	Actions.add("CommonActions.AboutQt", new QAction(app));
	Actions.add("CommonActions.OpenDVD", new TkAction(app, tr("Ctrl+D")));
	Actions.add("CommonActions.OpenURL", new TkAction(app, tr("Ctrl+U")));
	Actions.add("CommonActions.OpenVCD", new QAction(app));
	Actions.add("CommonActions.NewMediaObject", new QAction(app));
	Actions.add("CommonActions.Equalizer", new TkAction(app, tr("Ctrl+E")));
	Actions.add("CommonActions.Configure", new QAction(app));
	Actions.add("CommonActions.EmptyMenu", new QAction(app));

	TkAction * action = new TkAction(app, tr("Space"), Qt::Key_MediaPlay, Qt::Key_Pause);
	action->setShortcutContext(Qt::ApplicationShortcut);
	Actions.add("CommonActions.PlayPause", action);
	action = new TkAction(app, Qt::Key_MediaStop);
	Actions.add("CommonActions.Stop", action);
	action = new TkAction(app, tr("Ctrl+N"), tr(">"), Qt::Key_MediaNext);
	action->setShortcutContext(Qt::ApplicationShortcut);
	Actions.add("CommonActions.NextTrack", action);
	action = new TkAction(app, tr("Ctrl+P"), tr("<"), Qt::Key_MediaPrevious);
	action->setShortcutContext(Qt::ApplicationShortcut);
	Actions.add("CommonActions.PreviousTrack", action);

	action = new TkAction(app, tr("Left"));
	action->setShortcutContext(Qt::ApplicationShortcut);
	Actions.add("CommonActions.JumpBackward10s", action);
	action = new TkAction(app, tr("Ctrl+Left"));
	action->setShortcutContext(Qt::ApplicationShortcut);
	Actions.add("CommonActions.JumpBackward1min", action);
	action = new TkAction(app, tr("Shift+Left"));
	action->setShortcutContext(Qt::ApplicationShortcut);
	Actions.add("CommonActions.JumpBackward10min", action);

	action = new TkAction(app, tr("Right"));
	action->setShortcutContext(Qt::ApplicationShortcut);
	Actions.add("CommonActions.JumpForward10s", action);
	action = new TkAction(app, tr("Ctrl+Right"));
	action->setShortcutContext(Qt::ApplicationShortcut);
	Actions.add("CommonActions.JumpForward1min", action);
	action = new TkAction(app, tr("Shift+Right"));
	action->setShortcutContext(Qt::ApplicationShortcut);
	Actions.add("CommonActions.JumpForward10min", action);

	action = new TkAction(app, tr("["));
	action->setShortcutContext(Qt::ApplicationShortcut);
	Actions.add("CommonActions.SpeedDecrease10%", action);
	action = new TkAction(app, tr("]"));
	action->setShortcutContext(Qt::ApplicationShortcut);
	Actions.add("CommonActions.SpeedIncrease10%", action);

	action = new TkAction(app, tr("Ctrl+M"));
	action->setShortcutContext(Qt::ApplicationShortcut);
	action->setCheckable(true);
	Actions.add("CommonActions.VolumeMute", action);

	action = new TkAction(app, tr("Ctrl+Down"), tr("-"), tr("Alt+-"));
	action->setShortcutContext(Qt::ApplicationShortcut);
	Actions.add("CommonActions.VolumeDecrease10%", action);
	action = new TkAction(app, tr("Ctrl+Up"), tr("+"), tr("Alt++"));
	action->setShortcutContext(Qt::ApplicationShortcut);
	Actions.add("CommonActions.VolumeIncrease10%", action);

	action = new TkAction(app, tr("Ctrl+F"), tr("Alt+Return"));
	action->setShortcutContext(Qt::ApplicationShortcut);
	action->setCheckable(true);
	Actions.add("CommonActions.FullScreen", action);

	action = new TkAction(app, tr("Esc"));
	Actions.add("CommonActions.FullScreenExit", action);
}

void CommonActions::retranslate() {
	Actions["CommonActions.OpenFile"]->setText(tr("Play &File..."));
	Actions["CommonActions.OpenFile"]->setIcon(QIcon::fromTheme("document-open"));

	Actions["CommonActions.Quit"]->setText(tr("&Quit"));
	Actions["CommonActions.Quit"]->setIcon(QIcon::fromTheme("application-exit"));

	Actions["CommonActions.ReportBug"]->setText(tr("&Report a Problem..."));
	if (desktopEnvironment() == GNOME) {
		Actions["CommonActions.ReportBug"]->setIcon(QIcon::fromTheme("apport"));
	} else {
		Actions["CommonActions.ReportBug"]->setIcon(QIcon::fromTheme("tools-report-bug"));
	}

	Actions["CommonActions.ShowMailingList"]->setText(tr("&Discuss about QuarkPlayer..."));
	if (desktopEnvironment() == GNOME) {
		Actions["CommonActions.ShowMailingList"]->setIcon(QIcon::fromTheme("help-faq"));
	} else {
		Actions["CommonActions.ShowMailingList"]->setIcon(QIcon::fromTheme("help-hint"));
	}

	Actions["CommonActions.ShowLog"]->setText(tr("View &Log"));
	QIcon logIcon;
	if (desktopEnvironment() == GNOME) {
		logIcon = QIcon::fromTheme("logviewer");
	} else {
		logIcon = QIcon::fromTheme("text-x-log");
	}
	Actions["CommonActions.ShowLog"]->setIcon(logIcon);

	Actions["CommonActions.About"]->setText(tr("&About"));

	Actions["CommonActions.AboutQt"]->setText(tr("About &Qt"));

	Actions["CommonActions.OpenDVD"]->setText(tr("Play &DVD..."));
	Actions["CommonActions.OpenDVD"]->setIcon(QIcon::fromTheme("media-optical"));

	Actions["CommonActions.OpenURL"]->setText(tr("Play &URL..."));

	Actions["CommonActions.OpenVCD"]->setText(tr("Play &VCD..."));
\
	Actions["CommonActions.NewMediaObject"]->setText(tr("New Media Window"));
	Actions["CommonActions.NewMediaObject"]->setIcon(QIcon::fromTheme("tab-new"));

	Actions["CommonActions.Equalizer"]->setText(tr("&Equalizer..."));
	Actions["CommonActions.Equalizer"]->setIcon(QIcon::fromTheme("view-media-equalizer"));

	Actions["CommonActions.Configure"]->setText(tr("&Configure..."));
	Actions["CommonActions.Configure"]->setIcon(QIcon::fromTheme("preferences-system"));

	Actions["CommonActions.EmptyMenu"]->setText(tr("<empty>"));
	Actions["CommonActions.EmptyMenu"]->setEnabled(false);

	Actions["CommonActions.PreviousTrack"]->setText(tr("P&revious Track"));
	Actions["CommonActions.PreviousTrack"]->setIcon(QIcon::fromTheme("media-skip-backward"));

	Actions["CommonActions.PlayPause"]->setText(tr("&Play/Pause"));
	Actions["CommonActions.PlayPause"]->setIcon(QIcon::fromTheme("media-playback-start"));

	Actions["CommonActions.Stop"]->setText(tr("&Stop"));
	Actions["CommonActions.Stop"]->setIcon(QIcon::fromTheme("media-playback-stop"));

	Actions["CommonActions.NextTrack"]->setText(tr("&Next Track"));
	Actions["CommonActions.NextTrack"]->setIcon(QIcon::fromTheme("media-skip-forward"));

	Actions["CommonActions.JumpBackward10s"]->setText(tr("Jump &Backward 10s"));
	Actions["CommonActions.JumpBackward1min"]->setText(tr("Jump &Backward 1min"));
	Actions["CommonActions.JumpBackward1min"]->setIcon(QIcon::fromTheme("media-seek-backward"));
	Actions["CommonActions.JumpBackward10min"]->setText(tr("Jump &Backward 10min"));
	Actions["CommonActions.JumpForward10s"]->setText(tr("Jump &Forward 10s"));
	Actions["CommonActions.JumpForward1min"]->setText(tr("Jump &Forward 1min"));
	Actions["CommonActions.JumpForward1min"]->setIcon(QIcon::fromTheme("media-seek-forward"));
	Actions["CommonActions.JumpForward10min"]->setText(tr("Jump &Forward 10min"));
	Actions["CommonActions.SpeedDecrease10%"]->setText(tr("Decrease Speed"));
	Actions["CommonActions.SpeedIncrease10%"]->setText(tr("Increase Speed"));

	Actions["CommonActions.VolumeMute"]->setText(tr("&Mute"));
	Actions["CommonActions.VolumeMute"]->setIcon(QIcon::fromTheme("audio-volume-muted"));
	Actions["CommonActions.VolumeDecrease10%"]->setText(tr("&Decrease Volume"));
	Actions["CommonActions.VolumeIncrease10%"]->setText(tr("&Increase Volume"));

	Actions["CommonActions.FullScreen"]->setText(tr("&Fullscreen"));
	Actions["CommonActions.FullScreen"]->setIcon(QIcon::fromTheme("view-fullscreen"));

	Actions["CommonActions.FullScreenExit"]->setText(tr("&Exit Fullscreen"));
}

void CommonActions::stateChanged(Phonon::State newState) {
	//Enabled/disabled fullscreen button depending if media is a video or audio
	if (_quarkPlayer.currentMediaObject()->hasVideo()) {
		Actions["CommonActions.FullScreen"]->setEnabled(true);
	} else {
		Actions["CommonActions.FullScreen"]->setEnabled(false);
	}

	switch (newState) {
	case Phonon::ErrorState:
		break;

	case Phonon::PlayingState:
		Actions["CommonActions.PlayPause"]->setText(tr("&Pause"));
		Actions["CommonActions.PlayPause"]->setIcon(QIcon::fromTheme("media-playback-pause"));
		disconnect(Actions["CommonActions.PlayPause"], 0, 0, 0);
		connect(Actions["CommonActions.PlayPause"], SIGNAL(triggered()),
			_quarkPlayer.currentMediaObject(), SLOT(pause()));

		Actions["CommonActions.Stop"]->setEnabled(true);
		break;

	case Phonon::StoppedState:
		Actions["CommonActions.PlayPause"]->setText(tr("P&lay"));
		Actions["CommonActions.PlayPause"]->setIcon(QIcon::fromTheme("media-playback-start"));
		disconnect(Actions["CommonActions.PlayPause"], 0, 0, 0);
		connect(Actions["CommonActions.PlayPause"], SIGNAL(triggered()),
			_quarkPlayer.currentMediaObject(), SLOT(play()));

		Actions["CommonActions.Stop"]->setEnabled(false);
		break;

	case Phonon::PausedState:
		Actions["CommonActions.PlayPause"]->setText(tr("P&lay"));
		Actions["CommonActions.PlayPause"]->setIcon(QIcon::fromTheme("media-playback-start"));
		disconnect(Actions["CommonActions.PlayPause"], 0, 0, 0);
		connect(Actions["CommonActions.PlayPause"], SIGNAL(triggered()),
			_quarkPlayer.currentMediaObject(), SLOT(play()));

		Actions["CommonActions.Stop"]->setEnabled(true);
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
	disconnect(Actions["CommonActions.Stop"], 0, 0, 0);
	connect(Actions["CommonActions.Stop"], SIGNAL(triggered()),
		mediaObject, SLOT(stop()));
}
