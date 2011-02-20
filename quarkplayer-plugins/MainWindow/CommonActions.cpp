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

	ActionCollection::addAction("CommonActions.OpenFile", new TkAction(app, QKeySequence::Open));
	ActionCollection::addAction("CommonActions.Quit", new TkAction(app, tr("Ctrl+Q"), tr("Alt+X")));
	ActionCollection::addAction("CommonActions.ReportBug", new QAction(app));
	ActionCollection::addAction("CommonActions.ShowMailingList", new QAction(app));
	ActionCollection::addAction("CommonActions.ViewLog", new QAction(app));
	ActionCollection::addAction("CommonActions.About", new TkAction(app, tr("Ctrl+F1")));
	ActionCollection::addAction("CommonActions.AboutQt", new QAction(app));
	ActionCollection::addAction("CommonActions.OpenDVD", new TkAction(app, tr("Ctrl+D")));
	ActionCollection::addAction("CommonActions.OpenURL", new TkAction(app, tr("Ctrl+U")));
	ActionCollection::addAction("CommonActions.OpenVCD", new QAction(app));
	ActionCollection::addAction("CommonActions.NewMediaObject", new QAction(app));
	ActionCollection::addAction("CommonActions.Equalizer", new TkAction(app, tr("Ctrl+E")));
	ActionCollection::addAction("CommonActions.Configure", new QAction(app));
	ActionCollection::addAction("CommonActions.EmptyMenu", new QAction(app));

	TkAction * action = new TkAction(app, tr("Space"), Qt::Key_MediaPlay, Qt::Key_Pause);
	action->setShortcutContext(Qt::ApplicationShortcut);
	ActionCollection::addAction("CommonActions.PlayPause", action);
	action = new TkAction(app, Qt::Key_MediaStop);
	ActionCollection::addAction("CommonActions.Stop", action);
	action = new TkAction(app, tr("Ctrl+N"), tr(">"), Qt::Key_MediaNext);
	action->setShortcutContext(Qt::ApplicationShortcut);
	ActionCollection::addAction("CommonActions.NextTrack", action);
	action = new TkAction(app, tr("Ctrl+P"), tr("<"), Qt::Key_MediaPrevious);
	action->setShortcutContext(Qt::ApplicationShortcut);
	ActionCollection::addAction("CommonActions.PreviousTrack", action);

	action = new TkAction(app, tr("Left"));
	action->setShortcutContext(Qt::ApplicationShortcut);
	ActionCollection::addAction("CommonActions.JumpBackward10s", action);
	action = new TkAction(app, tr("Ctrl+Left"));
	action->setShortcutContext(Qt::ApplicationShortcut);
	ActionCollection::addAction("CommonActions.JumpBackward1min", action);
	action = new TkAction(app, tr("Shift+Left"));
	action->setShortcutContext(Qt::ApplicationShortcut);
	ActionCollection::addAction("CommonActions.JumpBackward10min", action);

	action = new TkAction(app, tr("Right"));
	action->setShortcutContext(Qt::ApplicationShortcut);
	ActionCollection::addAction("CommonActions.JumpForward10s", action);
	action = new TkAction(app, tr("Ctrl+Right"));
	action->setShortcutContext(Qt::ApplicationShortcut);
	ActionCollection::addAction("CommonActions.JumpForward1min", action);
	action = new TkAction(app, tr("Shift+Right"));
	action->setShortcutContext(Qt::ApplicationShortcut);
	ActionCollection::addAction("CommonActions.JumpForward10min", action);

	action = new TkAction(app, tr("["));
	action->setShortcutContext(Qt::ApplicationShortcut);
	ActionCollection::addAction("CommonActions.SpeedDecrease10%", action);
	action = new TkAction(app, tr("]"));
	action->setShortcutContext(Qt::ApplicationShortcut);
	ActionCollection::addAction("CommonActions.SpeedIncrease10%", action);

	action = new TkAction(app, tr("Ctrl+M"));
	action->setShortcutContext(Qt::ApplicationShortcut);
	action->setCheckable(true);
	ActionCollection::addAction("CommonActions.VolumeMute", action);

	action = new TkAction(app, tr("Ctrl+Down"), tr("-"), tr("Alt+-"));
	action->setShortcutContext(Qt::ApplicationShortcut);
	ActionCollection::addAction("CommonActions.VolumeDecrease10%", action);
	action = new TkAction(app, tr("Ctrl+Up"), tr("+"), tr("Alt++"));
	action->setShortcutContext(Qt::ApplicationShortcut);
	ActionCollection::addAction("CommonActions.VolumeIncrease10%", action);

	action = new TkAction(app, tr("Ctrl+F"), tr("Alt+Return"));
	action->setShortcutContext(Qt::ApplicationShortcut);
	action->setCheckable(true);
	ActionCollection::addAction("CommonActions.FullScreen", action);

	action = new TkAction(app, tr("Esc"));
	ActionCollection::addAction("CommonActions.FullScreenExit", action);
}

void CommonActions::retranslate() {
	ActionCollection::action("CommonActions.OpenFile")->setText(tr("Play &File..."));
	ActionCollection::action("CommonActions.OpenFile")->setIcon(QIcon::fromTheme("document-open"));

	ActionCollection::action("CommonActions.Quit")->setText(tr("&Quit"));
	ActionCollection::action("CommonActions.Quit")->setIcon(QIcon::fromTheme("application-exit"));

	ActionCollection::action("CommonActions.ReportBug")->setText(tr("&Report a Problem..."));
	if (desktopEnvironment() == GNOME) {
		ActionCollection::action("CommonActions.ReportBug")->setIcon(QIcon::fromTheme("apport"));
	} else {
		ActionCollection::action("CommonActions.ReportBug")->setIcon(QIcon::fromTheme("tools-report-bug"));
	}

	ActionCollection::action("CommonActions.ShowMailingList")->setText(tr("&Discuss about QuarkPlayer..."));
	if (desktopEnvironment() == GNOME) {
		ActionCollection::action("CommonActions.ShowMailingList")->setIcon(QIcon::fromTheme("help-faq"));
	} else {
		ActionCollection::action("CommonActions.ShowMailingList")->setIcon(QIcon::fromTheme("help-hint"));
	}

	ActionCollection::action("CommonActions.ViewLog")->setText(tr("View &Log"));
	QIcon logIcon;
	if (desktopEnvironment() == GNOME) {
		logIcon = QIcon::fromTheme("logviewer");
	} else {
		logIcon = QIcon::fromTheme("text-x-log");
	}
	ActionCollection::action("CommonActions.ViewLog")->setIcon(logIcon);

	ActionCollection::action("CommonActions.About")->setText(tr("&About"));
	ActionCollection::action("CommonActions.About")->setIcon(QIcon::fromTheme("help-about"));

	ActionCollection::action("CommonActions.AboutQt")->setText(tr("About &Qt"));
	ActionCollection::action("CommonActions.AboutQt")->setIcon(QIcon::fromTheme("help-about"));

	ActionCollection::action("CommonActions.OpenDVD")->setText(tr("Play &DVD..."));
	ActionCollection::action("CommonActions.OpenDVD")->setIcon(QIcon::fromTheme("media-optical"));

	ActionCollection::action("CommonActions.OpenURL")->setText(tr("Play &URL..."));
	ActionCollection::action("CommonActions.OpenURL")->setIcon(QIcon::fromTheme("document-open-remote"));

	ActionCollection::action("CommonActions.OpenVCD")->setText(tr("Play &VCD..."));
	//ActionCollection::action("CommonActions.OpenVCD")->setIcon(QIcon::fromTheme("media-optical"));

	ActionCollection::action("CommonActions.NewMediaObject")->setText(tr("New Media Window"));
	ActionCollection::action("CommonActions.NewMediaObject")->setIcon(QIcon::fromTheme("tab-new"));

	ActionCollection::action("CommonActions.Equalizer")->setText(tr("&Equalizer..."));
	ActionCollection::action("CommonActions.Equalizer")->setIcon(QIcon::fromTheme("view-media-equalizer"));

	ActionCollection::action("CommonActions.Configure")->setText(tr("&Configure..."));
	ActionCollection::action("CommonActions.Configure")->setIcon(QIcon::fromTheme("preferences-system"));

	ActionCollection::action("CommonActions.EmptyMenu")->setText(tr("<empty>"));
	ActionCollection::action("CommonActions.EmptyMenu")->setEnabled(false);

	ActionCollection::action("CommonActions.PreviousTrack")->setText(tr("P&revious Track"));
	ActionCollection::action("CommonActions.PreviousTrack")->setIcon(QIcon::fromTheme("media-skip-backward"));

	ActionCollection::action("CommonActions.PlayPause")->setText(tr("&Play/Pause"));
	ActionCollection::action("CommonActions.PlayPause")->setIcon(QIcon::fromTheme("media-playback-start"));

	ActionCollection::action("CommonActions.Stop")->setText(tr("&Stop"));
	ActionCollection::action("CommonActions.Stop")->setIcon(QIcon::fromTheme("media-playback-stop"));

	ActionCollection::action("CommonActions.NextTrack")->setText(tr("&Next Track"));
	ActionCollection::action("CommonActions.NextTrack")->setIcon(QIcon::fromTheme("media-skip-forward"));

	ActionCollection::action("CommonActions.JumpBackward10s")->setText(tr("Jump &Backward 10s"));
	ActionCollection::action("CommonActions.JumpBackward10s")->setIcon(QIcon::fromTheme("media-seek-backward"));
	ActionCollection::action("CommonActions.JumpBackward1min")->setText(tr("Jump &Backward 1min"));
	ActionCollection::action("CommonActions.JumpBackward1min")->setIcon(QIcon::fromTheme("media-seek-backward"));
	ActionCollection::action("CommonActions.JumpBackward10min")->setText(tr("Jump &Backward 10min"));
	ActionCollection::action("CommonActions.JumpBackward10min")->setIcon(QIcon::fromTheme("media-seek-backward"));
	ActionCollection::action("CommonActions.JumpForward10s")->setText(tr("Jump &Forward 10s"));
	ActionCollection::action("CommonActions.JumpForward10s")->setIcon(QIcon::fromTheme("media-seek-forward"));
	ActionCollection::action("CommonActions.JumpForward1min")->setText(tr("Jump &Forward 1min"));
	ActionCollection::action("CommonActions.JumpForward1min")->setIcon(QIcon::fromTheme("media-seek-forward"));
	ActionCollection::action("CommonActions.JumpForward10min")->setText(tr("Jump &Forward 10min"));
	ActionCollection::action("CommonActions.JumpForward10min")->setIcon(QIcon::fromTheme("media-seek-forward"));
	ActionCollection::action("CommonActions.SpeedDecrease10%")->setText(tr("Decrease Speed"));
	ActionCollection::action("CommonActions.SpeedDecrease10%")->setIcon(QIcon::fromTheme("media-seek-backward"));
	ActionCollection::action("CommonActions.SpeedIncrease10%")->setText(tr("Increase Speed"));
	ActionCollection::action("CommonActions.SpeedIncrease10%")->setIcon(QIcon::fromTheme("media-seek-forward"));

	ActionCollection::action("CommonActions.VolumeMute")->setText(tr("&Mute"));
	ActionCollection::action("CommonActions.VolumeMute")->setIcon(QIcon::fromTheme("audio-volume-muted"));
	ActionCollection::action("CommonActions.VolumeDecrease10%")->setText(tr("&Decrease Volume"));
	ActionCollection::action("CommonActions.VolumeDecrease10%")->setIcon(QIcon::fromTheme("audio-volume-low"));
	ActionCollection::action("CommonActions.VolumeIncrease10%")->setText(tr("&Increase Volume"));
	ActionCollection::action("CommonActions.VolumeIncrease10%")->setIcon(QIcon::fromTheme("audio-volume-high"));

	ActionCollection::action("CommonActions.FullScreen")->setText(tr("&Fullscreen"));
	ActionCollection::action("CommonActions.FullScreen")->setIcon(QIcon::fromTheme("view-fullscreen"));

	ActionCollection::action("CommonActions.FullScreenExit")->setText(tr("&Exit Fullscreen"));
}

void CommonActions::stateChanged(Phonon::State newState) {
	//Enabled/disabled fullscreen button depending if media is a video or audio
	if (_quarkPlayer.currentMediaObject()->hasVideo()) {
		ActionCollection::action("CommonActions.FullScreen")->setEnabled(true);
	} else {
		ActionCollection::action("CommonActions.FullScreen")->setEnabled(false);
	}

	switch (newState) {
	case Phonon::ErrorState:
		break;

	case Phonon::PlayingState:
		ActionCollection::action("CommonActions.PlayPause")->setText(tr("&Pause"));
		ActionCollection::action("CommonActions.PlayPause")->setIcon(QIcon::fromTheme("media-playback-pause"));
		disconnect(ActionCollection::action("CommonActions.PlayPause"), 0, 0, 0);
		connect(ActionCollection::action("CommonActions.PlayPause"), SIGNAL(triggered()),
			_quarkPlayer.currentMediaObject(), SLOT(pause()));

		ActionCollection::action("CommonActions.Stop")->setEnabled(true);
		break;

	case Phonon::StoppedState:
		ActionCollection::action("CommonActions.PlayPause")->setText(tr("P&lay"));
		ActionCollection::action("CommonActions.PlayPause")->setIcon(QIcon::fromTheme("media-playback-start"));
		disconnect(ActionCollection::action("CommonActions.PlayPause"), 0, 0, 0);
		connect(ActionCollection::action("CommonActions.PlayPause"), SIGNAL(triggered()),
			_quarkPlayer.currentMediaObject(), SLOT(play()));

		ActionCollection::action("CommonActions.Stop")->setEnabled(false);
		break;

	case Phonon::PausedState:
		ActionCollection::action("CommonActions.PlayPause")->setText(tr("P&lay"));
		ActionCollection::action("CommonActions.PlayPause")->setIcon(QIcon::fromTheme("media-playback-start"));
		disconnect(ActionCollection::action("CommonActions.PlayPause"), 0, 0, 0);
		connect(ActionCollection::action("CommonActions.PlayPause"), SIGNAL(triggered()),
			_quarkPlayer.currentMediaObject(), SLOT(play()));

		ActionCollection::action("CommonActions.Stop")->setEnabled(true);
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
	disconnect(ActionCollection::action("CommonActions.Stop"), 0, 0, 0);
	connect(ActionCollection::action("CommonActions.Stop"), SIGNAL(triggered()),
		mediaObject, SLOT(stop()));
}
