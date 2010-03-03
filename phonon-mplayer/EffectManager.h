/*
 * MPlayer backend for the Phonon library
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

#ifndef PHONON_MPLAYER_EFFECTMANAGER_H
#define PHONON_MPLAYER_EFFECTMANAGER_H

#include <phonon/effectinterface.h>
#include <phonon/effectparameter.h>

#include <QtCore/QObject>

namespace Phonon
{
namespace MPlayer
{

/**
 * A video or audio effect.
 *
 * @author Tanguy Krotoff
 */
class EffectInfo {
public:

	/** Type of effect. */
	enum Type {
		AudioEffect,
		VideoEffect
	};

	/**
	 * Constructs an effect given its name and its MPlayer command.
	 *
	 * @param name effect name
	 * @param command MPlayer command for the effect
	 * @param type effect type (audio or video effect)
	 */
	EffectInfo(const QString & name, const QString & command, Type type);

	~EffectInfo();

	/** Gets the effect name. */
	QString name() const;

	/** Gets the MPlayer command for the effect. */
	QString command() const;

	/** Gets the effet type (audio or video). */
	Type type() const;

private:

	QString _name;

	QString _command;

	Type _type;
};

/**
 * Keeps a list of available MPlayer effects.
 *
 * @author Tanguy Krotoff
 */
class EffectManager : public QObject {
	Q_OBJECT
public:

	/** Constructs the list of available MPlayer effects. */
	EffectManager(QObject * parent);
	~EffectManager();

	/** Gets the list of available effects. */
	QList<EffectInfo *> effectList() const;

private:

	QList<EffectInfo *> _effectList;
};

}}	//Namespace Phonon::MPlayer

#endif	//PHONON_MPLAYER_EFFECTMANAGER_H
