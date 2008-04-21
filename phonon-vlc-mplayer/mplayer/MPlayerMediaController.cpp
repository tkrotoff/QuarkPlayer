/*
 * VLC and MPlayer backends for the Phonon library
 * Copyright (C) 2007-2008  Tanguy Krotoff <tkrotoff@gmail.com>
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

#include "MPlayerMediaController.h"

namespace Phonon
{
namespace VLC_MPlayer
{

MPlayerMediaController::MPlayerMediaController()
	: MediaController() {
}

MPlayerMediaController::~MPlayerMediaController() {
}

//AudioStream
void MPlayerMediaController::audioStreamAdded(int id, const QString & lang) {
	qDebug() << __FUNCTION__;

	QHash<QByteArray, QVariant> properties;
	properties.insert("name", lang);
	properties.insert("description", lang);

	_availableAudioStreams << Phonon::AudioStreamDescription(id, properties);
}

void MPlayerMediaController::setCurrentAudioStream(const Phonon::AudioStreamDescription & stream) {
	qDebug() << __FUNCTION__;

	_process->writeToStdin("switch_audio " + QString::number(stream.index()));
}

QList<Phonon::AudioStreamDescription> MPlayerMediaController::availableAudioStreams() const {
	return _availableAudioStreams;
}

Phonon::AudioStreamDescription MPlayerMediaController::currentAudioStream() const {
	//TODO
	return _availableAudioStreams[0];
}

//SubtitleStream
void MPlayerMediaController::subtitleStreamAdded(int id, const QString & lang, const QString & type) {
	qDebug() << __FUNCTION__;

	QHash<QByteArray, QVariant> properties;
	properties.insert("name", lang);
	properties.insert("description", lang);
	properties.insert("type", type);

	_availableSubtitleStreams << Phonon::SubtitleStreamDescription(id, properties);
}

void MPlayerMediaController::setCurrentSubtitleStream(const Phonon::SubtitleStreamDescription & stream) {
	qDebug() << __FUNCTION__;

	_process->writeToStdin("sub_demux " + QString::number(stream.index()));
}

QList<Phonon::SubtitleStreamDescription> MPlayerMediaController::availableSubtitleStreams() const {
	return _availableSubtitleStreams;
}

Phonon::SubtitleStreamDescription MPlayerMediaController::currentSubtitleStream() const {
	//TODO
	return _availableSubtitleStreams[0];
}

}}	//Namespace Phonon::VLC_MPlayer
