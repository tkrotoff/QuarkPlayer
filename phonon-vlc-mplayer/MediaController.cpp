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

#include "MediaController.h"

namespace Phonon
{
namespace VLC_MPlayer
{

MediaController::MediaController() {
	_currentAngle = 0;
	_availableAngles = 0;

	_currentChapter = 0;
	_availableChapters = 0;

	_currentTitle = 0;
	_availableTitles = 0;

	_autoplayTitles = false;
}

MediaController::~MediaController() {
}

bool MediaController::hasInterface(Interface iface) const {
	switch (iface) {
	case AddonInterface::NavigationInterface:
		return true;
		break;
	case AddonInterface::ChapterInterface:
		return true;
		break;
	case AddonInterface::AngleInterface:
		return true;
		break;
	case AddonInterface::TitleInterface:
		return true;
		break;
	case AddonInterface::SubtitleInterface:
		return true;
		break;
	case AddonInterface::AudioChannelInterface:
		return true;
		break;
	default:
		qCritical() << __FUNCTION__ << "Error: unsupported AddonInterface::Interface" << iface;
	}

	return false;
}

QVariant MediaController::interfaceCall(Interface iface, int command, const QList<QVariant> & arguments) {
	switch (iface) {

	case AddonInterface::ChapterInterface:
		switch (static_cast<AddonInterface::ChapterCommand>(command)) {
			case AddonInterface::availableChapters:
				return availableChapters();
			case AddonInterface::chapter:
				return currentChapter();
			case AddonInterface::setChapter:
				if (arguments.isEmpty() || !arguments.first().canConvert(QVariant::Int)) {
					qCritical() << __FUNCTION__ << "Error: arguments invalid";
					return false;
				}
				setCurrentChapter(arguments.first().toInt());
				return true;
			default:
				qCritical() << __FUNCTION__ << "Error: unsupported AddonInterface::ChapterInterface command:" << command;
		}
		break;

	case AddonInterface::TitleInterface:
		switch (static_cast<AddonInterface::TitleCommand>(command)) {
			case AddonInterface::availableTitles:
				return availableTitles();
			case AddonInterface::title:
				return currentTitle();
			case AddonInterface::setTitle:
				if (arguments.isEmpty() || !arguments.first().canConvert(QVariant::Int)) {
					qCritical() << __FUNCTION__ << "Error: arguments invalid";
					return false;
				}
				setCurrentTitle(arguments.first().toInt());
				return true;
			case AddonInterface::autoplayTitles:
				return autoplayTitles();
			case AddonInterface::setAutoplayTitles:
				if (arguments.isEmpty() || !arguments.first().canConvert(QVariant::Bool)) {
					qCritical() << __FUNCTION__ << "Error: arguments invalid";
					return false;
				}
				setAutoplayTitles(arguments.first().toBool());
				return true;
			default:
				qCritical() << __FUNCTION__ << "Error: unsupported AddonInterface::TitleInterface command:" << command;
		}
		break;

	case AddonInterface::AngleInterface:
		switch (static_cast<AddonInterface::AngleCommand>(command)) {
			case AddonInterface::availableAngles:
			case AddonInterface::angle:
			case AddonInterface::setAngle:
				break;
			default:
				qCritical() << __FUNCTION__ << "Error: unsupported AddonInterface::AngleInterface command:" << command;
		}
		break;

	case AddonInterface::SubtitleInterface:
		switch (static_cast<AddonInterface::SubtitleCommand>(command)) {
			case AddonInterface::availableSubtitleStreams:
				return QVariant::fromValue(availableSubtitleStreams());

			case AddonInterface::currentSubtitleStream:
				return QVariant::fromValue(currentSubtitleStream());

			case AddonInterface::setCurrentSubtitleStream:
				if (arguments.isEmpty() || !arguments.first().canConvert<SubtitleStreamDescription>()) {
					qCritical() << __FUNCTION__ << "Error: arguments invalid";
					return false;
				}
				setCurrentSubtitleStream(arguments.first().value<SubtitleStreamDescription>());
				return true;

			default:
				qCritical() << __FUNCTION__ << "Error: unsupported AddonInterface::SubtitleInterface command:" << command;
		}
		break;

	case AddonInterface::AudioChannelInterface:
		switch (static_cast<AddonInterface::AudioChannelCommand>(command)) {

			case AddonInterface::availableAudioStreams:
				return QVariant::fromValue(availableAudioStreams());

			case AddonInterface::currentAudioStream:
				return QVariant::fromValue(currentAudioStream());

			case AddonInterface::setCurrentAudioStream:
				if (arguments.isEmpty() || !arguments.first().canConvert<AudioStreamDescription>()) {
					qCritical() << __FUNCTION__ << "Error: arguments invalid";
					return false;
				}
				setCurrentAudioStream(arguments.first().value<AudioStreamDescription>());
				return true;

			default:
				qCritical() << __FUNCTION__ << "Error: unsupported AddonInterface::AudioChannelInterface command:" << command;
		}
		break;

	default:
		qCritical() << __FUNCTION__ << "Error: unsupported AddonInterface::Interface:" << iface;
	}

	return new QVariant();
}

}}	//Namespace Phonon::VLC_MPlayer
