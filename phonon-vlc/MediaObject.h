/*
 * VLC backend for the Phonon library
 * Copyright (C) 2007-2008  Tanguy Krotoff <tkrotoff@gmail.com>
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

#ifndef PHONON_VLC_MEDIAOBJECT_H
#define PHONON_VLC_MEDIAOBJECT_H

#include <phonon/mediaobjectinterface.h>
#include <phonon/addoninterface.h>

#include <QtCore/QObject>

namespace Phonon
{
namespace VLC
{

class VLCMediaObject;

/**
 *
 *
 * @author Tanguy Krotoff
 */
class MediaObject : public QObject, public MediaObjectInterface, public AddonInterface {
	Q_OBJECT
	Q_INTERFACES(Phonon::MediaObjectInterface Phonon::AddonInterface)
public:

	MediaObject(QObject * parent);
	~MediaObject();

	void play();
	void pause();
	void stop();
	void seek(qint64 milliseconds);

	qint32 tickInterval() const;
	void setTickInterval(qint32 interval);

	bool hasVideo() const;
	bool isSeekable() const;
	qint64 currentTime() const;
	Phonon::State state() const;
	QString errorString() const;
	Phonon::ErrorType errorType() const;
	qint64 totalTime() const;
	MediaSource source() const;
	void setSource(const MediaSource & source);
	void setNextSource(const MediaSource & source);

	qint32 prefinishMark() const;
	void setPrefinishMark(qint32);

	qint32 transitionTime() const;
	void setTransitionTime(qint32);

	//From AddonInterface
	bool hasInterface(Interface iface) const;

	//From AddonInterface
	QVariant interfaceCall(Interface iface, int command, const QList<QVariant> & arguments = QList<QVariant>());

signals:

	//void aboutToFinish()
	//void bufferStatus(int percentFilled);
	void currentSourceChanged(const MediaSource & newSource);
	void finished();
	void hasVideoChanged(bool hasVideo);
	void metaDataChanged(const QMultiMap<QString, QString> & metaData);
	//void prefinishMarkReached(qint32 msecToEnd);
	void seekableChanged(bool isSeekable);
	void stateChanged(Phonon::State newState, Phonon::State oldState);
	void tick(qint64 time);
	void totalTimeChanged(qint64 newTotalTime);

private slots:

	void stateChangedInternal(Phonon::State newState);

	void metaDataChangedInternal(const QMultiMap<QString, QString> & metaData);

private:

	void loadMediaInternal(const QString & filename);
	void playInternal(const QString & filename);

	void resume();

	MediaSource _mediaSource;

	VLCMediaObject * _vlcMediaObject;

	Phonon::State _currentState;
};

}}	//Namespace Phonon::VLC

#endif	//PHONON_VLC_MEDIAOBJECT_H
