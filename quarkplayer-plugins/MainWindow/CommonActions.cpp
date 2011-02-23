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

	Actions::add("CommonActions.OpenFile", new TkAction(app, QKeySequence::Open));
	Actions::add("CommonActions.Quit", new TkAction(app, tr("Ctrl+Q"), tr("Alt+X")));
	Actions::add("CommonActions.ReportBug", new QAction(app));
	Actions::add("CommonActions.ShowMailingList", new QAction(app));
	Actions::add("CommonActions.ShowLog", new QAction(app));
	Actions::add("CommonActions.About", new TkAction(app, tr("Ctrl+F1")));
	Actions::add("CommonActions.AboutQt", new QAction(app));
	Actions::add("CommonActions.OpenDVD", new TkAction(app, tr("Ctrl+D")));
	Actions::add("CommonActions.OpenURL", new TkAction(app, tr("Ctrl+U")));
	Actions::add("CommonActions.OpenVCD", new QAction(app));
	Actions::add("CommonActions.NewMediaObject", new QAction(app));
	Actions::add("CommonActions.Equalizer", new TkAction(app, tr("Ctrl+E")));
	Actions::add("CommonActions.Configure", new QAction(app));
	Actions::add("CommonActions.EmptyMenu", new QAction(app));

	TkAction * action = new TkAction(app, tr("Space"), Qt::Key_MediaPlay, Qt::Key_Pause);
	action->setShortcutContext(Qt::ApplicationShortcut);
	Actions::add("CommonActions.PlayPause", action);
	action = new TkAction(app, Qt::Key_MediaStop);
	Actions::add("CommonActions.Stop", action);
	action = new TkAction(app, tr("Ctrl+N"), tr(">"), Qt::Key_MediaNext);
	action->setShortcutContext(Qt::ApplicationShortcut);
	Actions::add("CommonActions.NextTrack", action);
	action = new TkAction(app, tr("Ctrl+P"), tr("<"), Qt::Key_MediaPrevious);
	action->setShortcutContext(Qt::ApplicationShortcut);
	Actions::add("CommonActions.PreviousTrack", action);

	action = new TkAction(app, tr("Left"));
	action->setShortcutContext(Qt::ApplicationShortcut);
	Actions::add("CommonActions.JumpBackward10s", action);
	action = new TkAction(app, tr("Ctrl+Left"));
	action->setShortcutContext(Qt::ApplicationShortcut);
	Actions::add("CommonActions.JumpBackward1min", action);
	action = new TkAction(app, tr("Shift+Left"));
	action->setShortcutContext(Qt::ApplicationShortcut);
	Actions::add("CommonActions.JumpBackward10min", action);

	action = new TkAction(app, tr("Right"));
	action->setShortcutContext(Qt::ApplicationShortcut);
	Actions::add("CommonActions.JumpForward10s", action);
	action = new TkAction(app, tr("Ctrl+Right"));
	action->setShortcutContext(Qt::ApplicationShortcut);
	Actions::add("CommonActions.JumpForward1min", action);
	action = new TkAction(app, tr("Shift+Right"));
	action->setShortcutContext(Qt::ApplicationShortcut);
	Actions::add("CommonActions.JumpForward10min", action);

	action = new TkAction(app, tr("["));
	action->setShortcutContext(Qt::ApplicationShortcut);
	Actions::add("CommonActions.SpeedDecrease10%", action);
	action = new TkAction(app, tr("]"));
	action->setShortcutContext(Qt::ApplicationShortcut);
	Actions::add("CommonActions.SpeedIncrease10%", action);

	action = new TkAction(app, tr("Ctrl+M"));
	action->setShortcutContext(Qt::ApplicationShortcut);
	action->setCheckable(true);
	Actions::add("CommonActions.VolumeMute", action);

	action = new TkAction(app, tr("Ctrl+Down"), tr("-"), tr("Alt+-"));
	action->setShortcutContext(Qt::ApplicationShortcut);
	Actions::add("CommonActions.VolumeDecrease10%", action);
	action = new TkAction(app, tr("Ctrl+Up"), tr("+"), tr("Alt++"));
	action->setShortcutContext(Qt::ApplicationShortcut);
	Actions::add("CommonActions.VolumeIncrease10%", action);

	action = new TkAction(app, tr("Ctrl+F"), tr("Alt+Return"));
	action->setShortcutContext(Qt::ApplicationShortcut);
	action->setCheckable(true);
	Actions::add("CommonActions.FullScreen", action);

	action = new TkAction(app, tr("Esc"));
	Actions::add("CommonActions.FullScreenExit", action);
}

void CommonActions::retranslate() {
	Actions::get("CommonActions.OpenFile")->setText(tr("Play &File..."));
	Actions::get("CommonActions.OpenFile")->setIcon(QIcon::fromTheme("document-open"));

	Actions::get("CommonActions.Quit")->setText(tr("&Quit"));
	Actions::get("CommonActions.Quit")->setIcon(QIcon::fromTheme("application-exit"));

	Actions::get("CommonActions.ReportBug")->setText(tr("&Report a Problem..."));
	if (desktopEnvironment() == GNOME) {
		Actions::get("CommonActions.ReportBug")->setIcon(QIcon::fromTheme("apport"));
	} else {
		Actions::get("CommonActions.ReportBug")->setIcon(QIcon::fromTheme("tools-report-bug"));
	}

	Actions::get("CommonActions.ShowMailingList")->setText(tr("&Discuss about QuarkPlayer..."));
	if (desktopEnvironment() == GNOME) {
		Actions::get("CommonActions.ShowMailingList")->setIcon(QIcon::fromTheme("help-faq"));
	} else {
		Actions::get("CommonActions.ShowMailingList")->setIcon(QIcon::fromTheme("help-hint"));
	}

	Actions::get("CommonActions.ShowLog")->setText(tr("View &Log"));
	QIcon logIcon;
	if (desktopEnvironment() == GNOME) {
		logIcon = QIcon::fromTheme("logviewer");
	} else {
		logIcon = QIcon::fromTheme("text-x-log");
	}
	Actions::get("CommonActions.ShowLog")->setIcon(logIcon);

	Actions::get("CommonActions.About")->setText(tr("&About"));

	Actions::get("CommonActions.AboutQt")->setText(tr("About &Qt"));

	Actions::get("CommonActions.OpenDVD")->setText(tr("Play &DVD..."));
	Actions::get("CommonActions.OpenDVD")->setIcon(QIcon::fromTheme("media-optical"));

	Actions::get("CommonActions.OpenURL")->setText(tr("Play &URL..."));

	Actions::get("CommonActions.OpenVCD")->setText(tr("Play &VCD..."));
\
	Actions::get("CommonActions.NewMediaObject")->setText(tr("New Media Window"));
	Actions::get("CommonActions.NewMediaObject")->setIcon(QIcon::fromTheme("tab-new"));

	Actions::get("CommonActions.Equalizer")->setText(tr("&Equalizer..."));
	Actions::get("CommonActions.Equalizer")->setIcon(QIcon::fromTheme("view-media-equalizer"));

	Actions::get("CommonActions.Configure")->setText(tr("&Configure..."));
	Actions::get("CommonActions.Configure")->setIcon(QIcon::fromTheme("preferences-system"));

	Actions::get("CommonActions.EmptyMenu")->setText(tr("<empty>"));
	Actions::get("CommonActions.EmptyMenu")->setEnabled(false);

	Actions::get("CommonActions.PreviousTrack")->setText(tr("P&revious Track"));
	Actions::get("CommonActions.PreviousTrack")->setIcon(QIcon::fromTheme("media-skip-backward"));

	Actions::get("CommonActions.PlayPause")->setText(tr("&Play/Pause"));
	Actions::get("CommonActions.PlayPause")->setIcon(QIcon::fromTheme("media-playback-start"));

	Actions::get("CommonActions.Stop")->setText(tr("&Stop"));
	Actions::get("CommonActions.Stop")->setIcon(QIcon::fromTheme("media-playback-stop"));

	Actions::get("CommonActions.NextTrack")->setText(tr("&Next Track"));
	Actions::get("CommonActions.NextTrack")->setIcon(QIcon::fromTheme("media-skip-forward"));

	Actions::get("CommonActions.JumpBackward10s")->setText(tr("Jump &Backward 10s"));
	Actions::get("CommonActions.JumpBackward1min")->setText(tr("Jump &Backward 1min"));
	Actions::get("CommonActions.JumpBackward1min")->setIcon(QIcon::fromTheme("media-seek-backward"));
	Actions::get("CommonActions.JumpBackward10min")->setText(tr("Jump &Backward 10min"));
	Actions::get("CommonActions.JumpForward10s")->setText(tr("Jump &Forward 10s"));
	Actions::get("CommonActions.JumpForward1min")->setText(tr("Jump &Forward 1min"));
	Actions::get("CommonActions.JumpForward1min")->setIcon(QIcon::fromTheme("media-seek-forward"));
	Actions::get("CommonActions.JumpForward10min")->setText(tr("Jump &Forward 10min"));
	Actions::get("CommonActions.SpeedDecrease10%")->setText(tr("Decrease Speed"));
	Actions::get("CommonActions.SpeedIncrease10%")->setText(tr("Increase Speed"));

	Actions::get("CommonActions.VolumeMute")->setText(tr("&Mute"));
	Actions::get("CommonActions.VolumeMute")->setIcon(QIcon::fromTheme("audio-volume-muted"));
	Actions::get("CommonActions.VolumeDecrease10%")->setText(tr("&Decrease Volume"));
	Actions::get("CommonActions.VolumeIncrease10%")->setText(tr("&Increase Volume"));

	Actions::get("CommonActions.FullScreen")->setText(tr("&Fullscreen"));
	Actions::get("CommonActions.FullScreen")->setIcon(QIcon::fromTheme("view-fullscreen"));

	Actions::get("CommonActions.FullScreenExit")->setText(tr("&Exit Fullscreen"));
}

void CommonActions::stateChanged(Phonon::State newState) {
	//Enabled/disabled fullscreen button depending if media is a video or audio
	if (_quarkPlayer.currentMediaObject()->hasVideo()) {
		Actions::get("CommonActions.FullScreen")->setEnabled(true);
	} else {
		Actions::get("CommonActions.FullScreen")->setEnabled(false);
	}

	switch (newState) {
	case Phonon::ErrorState:
		break;

	case Phonon::PlayingState:
		Actions::get("CommonActions.PlayPause")->setText(tr("&Pause"));
		Actions::get("CommonActions.PlayPause")->setIcon(QIcon::fromTheme("media-playback-pause"));
		disconnect(Actions::get("CommonActions.PlayPause"), 0, 0, 0);
		connect(Actions::get("CommonActions.PlayPause"), SIGNAL(triggered()),
			_quarkPlayer.currentMediaObject(), SLOT(pause()));

		Actions::get("CommonActions.Stop")->setEnabled(true);
		break;

	case Phonon::StoppedState:
		Actions::get("CommonActions.PlayPause")->setText(tr("P&lay"));
		Actions::get("CommonActions.PlayPause")->setIcon(QIcon::fromTheme("media-playback-start"));
		disconnect(Actions::get("CommonActions.PlayPause"), 0, 0, 0);
		connect(Actions::get("CommonActions.PlayPause"), SIGNAL(triggered()),
			_quarkPlayer.currentMediaObject(), SLOT(play()));

		Actions::get("CommonActions.Stop")->setEnabled(false);
		break;

	case Phonon::PausedState:
		Actions::get("CommonActions.PlayPause")->setText(tr("P&lay"));
		Actions::get("CommonActions.PlayPause")->setIcon(QIcon::fromTheme("media-playback-start"));
		disconnect(Actions::get("CommonActions.PlayPause"), 0, 0, 0);
		connect(Actions::get("CommonActions.PlayPause"), SIGNAL(triggered()),
			_quarkPlayer.currentMediaObject(), SLOT(play()));

		Actions::get("CommonActions.Stop")->setEnabled(true);
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
	disconnect(Actions::get("CommonActions.Stop"), 0, 0, 0);
	connect(Actions::get("CommonActions.Stop"), SIGNAL(triggered()),
		mediaObject, SLOT(stop()));
}
