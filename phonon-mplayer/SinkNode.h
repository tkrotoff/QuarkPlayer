/*
 * MPlayer backend for the Phonon library
 * Copyright (C) 2007-2010  Tanguy Krotoff <tkrotoff@gmail.com>
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

#ifndef PHONON_MPLAYER_SINKNODE_H
#define PHONON_MPLAYER_SINKNODE_H

#include <QtCore/QObject>
#include <QtCore/QString>

namespace Phonon
{
namespace MPlayer
{

class MediaObject;

/**
 * Connects nodes together: a source to a sink.
 *
 * @see http://qt.nokia.com/doc/4.6/phonon-overview.html#sinks
 * @author Tanguy Krotoff
 */
class SinkNode : public QObject {
	Q_OBJECT
public:

	SinkNode(QObject * parent);
	virtual ~SinkNode();

	/**
	 * Connects the sink to a MediaObject.
	 *
	 * The sink can be connected to a single MediaObject.
	 */
	virtual void connectToMediaObject(MediaObject * mediaObject);

	/**
	 * Disconnects the sink from a MediaObject.
	 *
	 * The sink can be connected to a single MediaObject.
	 */
	virtual void disconnectFromMediaObject(MediaObject * mediaObject);

protected:

	/**
	 * Sends a command to the MPlayer process.
	 *
	 * This is factorization code, simplify MPlayer command writing.
	 *
	 * @return true if the command was sent; false if the MPlayer process is not running
	 * @see MPlayerProcess
	 */
	bool sendMPlayerCommand(const QString & command) const;

	MediaObject * _mediaObject;

private:

};

}}	//Namespace Phonon::MPlayer

#endif	//PHONON_MPLAYER_SINKNODE_H
