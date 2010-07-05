/*
 * MPlayer backend for the Phonon library
 * Copyright (C) 2007-2009  Tanguy Krotoff <tkrotoff@gmail.com>
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

#include "Effect.h"

#include "EffectManager.h"

#include "MediaObject.h"

#include "libmplayer/MPlayerLoader.h"
#include "libmplayer/MediaSettings.h"
#include "libmplayer/MPlayerProcess.h"

namespace Phonon
{
namespace MPlayer
{

Effect::Effect(EffectManager * effectManager, int effectId, QObject * parent)
	: SinkNode(parent) {

	QList<EffectInfo *> effects = effectManager->effectList();
	if (effectId >= 0 && effectId < effects.size()) {
		_effectCommand = effects[effectId]->command();
		_effectType = effects[effectId]->type();
	} else {
		//Effect ID out of range
		Q_ASSERT(0);
	}
}

Effect::~Effect() {
}

void Effect::connectToMediaObject(MediaObject * mediaObject) {
	SinkNode::connectToMediaObject(mediaObject);

	switch (_effectType) {
	case EffectInfo::AudioEffect:
		MPlayerLoader::settings.audioFilters.append(_effectCommand);
		break;
	case EffectInfo::VideoEffect:
		MPlayerLoader::settings.videoFilters.append(_effectCommand);
		break;
	}

	MPlayerProcess * process = _mediaObject->getMPlayerProcess();
	MPlayerLoader::restart(process);
}

void Effect::disconnectFromMediaObject(MediaObject * mediaObject) {
	SinkNode::disconnectFromMediaObject(mediaObject);

	switch (_effectType) {
	case EffectInfo::AudioEffect:
		MPlayerLoader::settings.audioFilters.removeAll(_effectCommand);
		break;
	case EffectInfo::VideoEffect:
		MPlayerLoader::settings.videoFilters.removeAll(_effectCommand);
		break;
	}

	MPlayerProcess * process = _mediaObject->getMPlayerProcess();
	MPlayerLoader::restart(process);
}

QList<EffectParameter> Effect::parameters() const {
	QList<EffectParameter> params;
	return params;
}

QVariant Effect::parameterValue(const EffectParameter & param) const {
	Q_UNUSED(param);
	return QVariant();
}

void Effect::setParameterValue(const EffectParameter & param, const QVariant & newValue) {
	Q_UNUSED(param);
	Q_UNUSED(newValue);
}

}}	//Namespace Phonon::MPlayer
