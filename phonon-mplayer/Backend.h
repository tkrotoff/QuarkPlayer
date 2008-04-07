/*
 * MPlayer backend for the Phonon library
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

#ifndef PHONON_MPLAYER_BACKEND_H
#define PHONON_MPLAYER_BACKEND_H

#include <phonon/objectdescription.h>
#include <phonon/backendinterface.h>

#include <QtCore/QList>
#include <QtCore/QPointer>
#include <QtCore/QStringList>
#include <QtCore/QObject>

class Core;
class MplayerWindow;

namespace Phonon
{
namespace MPlayer
{

/**
 * MPlayer backend class.
 *
 * @author Tanguy Krotoff
 */
class Backend : public QObject, public BackendInterface {
	Q_OBJECT
	Q_INTERFACES(Phonon::BackendInterface)
public:

	Backend(QObject * parent = NULL, const QVariantList & args = QVariantList());
	~Backend();

	QObject * createObject(BackendInterface::Class, QObject * parent, const QList<QVariant> & args);

	bool supportsVideo() const;
	bool supportsOSD() const;
	bool supportsFourcc(quint32 fourcc) const;
	bool supportsSubtitles() const;

	void freeSoundcardDevices();

	QList<int> objectDescriptionIndexes(ObjectDescriptionType) const;
	QHash<QByteArray, QVariant> objectDescriptionProperties(ObjectDescriptionType, int) const;

	bool startConnectionChange(QSet<QObject *>);
	bool connectNodes(QObject *, QObject *);
	bool disconnectNodes(QObject *, QObject *);
	bool endConnectionChange(QSet<QObject *>);

	/**
	 * Internal.
	 * Ugly hack to get SMPlayer core accessible from everywhere.
	 *
	 * Global variable.
	 */
	static Core * getSMPlayerCore();

	/**
	 * Internal.
	 * Ugly hack to get SMPlayer window accessible from everywhere.
	 *
	 * Global variable.
	 */
	static MplayerWindow * getSMPlayerWindow();

public slots:

	/**
	 * @see http://en.wikipedia.org/wiki/Mime_type
	 */
	QStringList availableMimeTypes() const;

	/**
	 * Test of introspection.
	 */
	QString toString() const;

signals:

	void objectDescriptionChanged(ObjectDescriptionType);

private:

	mutable QStringList m_supportedMimeTypes;

	/** Hack, global variable. */
	static Core * m_smplayerCore;

	/** Hack, global variable. */
	static MplayerWindow * m_smplayerWindow;
};

}}	//Namespace Phonon::MPlayer

#endif	//PHONON_MPLAYER_BACKEND_H
