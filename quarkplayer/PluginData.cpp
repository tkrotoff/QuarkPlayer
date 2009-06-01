/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2009  Tanguy Krotoff <tkrotoff@gmail.com>
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

#include "PluginData.h"
#include "config/Config.h"

//Important for deleteInstance()
//otherwise deleteInstance() won't do anything
//without knowing the type cf compiler warning
#include "PluginInterface.h"
///

#include <QtCore/QDir>
#include <QtCore/QStringList>
#include <QtCore/QDebug>

PluginData::PluginData(const QString & filename, const QUuid & uuid, bool enabled) {
	_filename = filename;
	_uuid = uuid;
	_enabled = enabled;

	_factory = NULL;
	_interface = NULL;
}

PluginData::PluginData(const PluginData & pluginData) {
	copy(pluginData);
}

PluginData::PluginData() {
	_enabled = false;

	_factory = NULL;
	_interface = NULL;
}

PluginData::~PluginData() {
}

void PluginData::copy(const PluginData & pluginData) {
	_filename = pluginData._filename;
	_uuid = pluginData._uuid;
	_enabled = pluginData._enabled;

	_interface = pluginData._interface;
	_factory = pluginData._factory;
}

PluginData & PluginData::operator=(const PluginData & pluginData) {
	//Handle self-assignment
	if (this == &pluginData) {
		return *this;
	}

	copy(pluginData);
	return *this;
}

int PluginData::operator==(const PluginData & pluginData) {
	return _uuid == pluginData._uuid;
}

QString PluginData::fileName() const {
	return _filename;
}

QUuid PluginData::uuid() const {
	return _uuid;
}

bool PluginData::isEnabled() const {
	return _enabled;
}

void PluginData::setEnabled(bool enabled) {
	_enabled = enabled;
}

PluginFactory * PluginData::factory() const {
	return _factory;
}

void PluginData::setFactory(PluginFactory * factory) {
	_factory = factory;
}

PluginInterface * PluginData::interface() const {
	return _interface;
}

void PluginData::setInterface(PluginInterface * interface) {
	_interface = interface;
}

void PluginData::deleteInterface() {
	if (_interface) {
		delete _interface;
		_interface = NULL;
	} else {
		qCritical() << __FUNCTION__ << "Error: _internal is NULL";
	}
}

PluginDataList PluginDataList::values(const QString & filename) const {
	PluginDataList pluginDataList;
	foreach(PluginData pluginData, *this) {
		if (pluginData.fileName() == filename) {
			pluginDataList += pluginData;
		}
	}
	return pluginDataList;
}


static const quint32 PLUGINLIST_HEADER_MAGIC = 0xA0B0C0D0;
static const quint32 PLUGINLIST_HEADER_VERSION = 1;

QDataStream & operator<<(QDataStream & stream, const PluginDataList & plugins) {
	//Write a header with a "magic number" and a version
	stream << PLUGINLIST_HEADER_MAGIC;
	stream << PLUGINLIST_HEADER_VERSION;

	foreach (PluginData pluginData, plugins) {
		if (pluginData.fileName().isEmpty()) {
			//FIXME don't why, stream can contain empty datas
			//even if we didn't put empty datas in it (!)
			qCritical() << __FUNCTION__ << "Error: empty pluginData:" << pluginData.uuid();
		} else {
			stream << pluginData.fileName();
			stream << pluginData.uuid();
			stream << pluginData.isEnabled();
		}
	}
	return stream;
}

QDataStream & operator>>(QDataStream & stream, PluginDataList & plugins) {
	if (stream.atEnd()) {
		//Empty stream
		return stream;
	}

	//Read and check the header
	quint32 magic;
	stream >> magic;
	if (magic != PLUGINLIST_HEADER_MAGIC) {
		qCritical() << __FUNCTION__ << "Error: wrong magic number:" << magic;
		return stream;
	}

	//Read the version
	quint32 version;
	stream >> version;
	if (version != PLUGINLIST_HEADER_VERSION) {
		qCritical() << __FUNCTION__ << "Error: wrong version number:" << version;
		return stream;
	}

	while (!stream.atEnd()) {
		QString filename;
		stream >> filename;

		QUuid uuid;
		bool enabled;
		stream >> uuid;
		stream >> enabled;

		if (filename.isEmpty()) {
			//FIXME don't know why, stream can contain empty datas
			//even if we didn't put empty datas in it (!)
		} else {
			PluginData pluginData(filename, uuid, enabled);
			plugins.removeAll(pluginData);
			plugins += pluginData;
		}
	}
	return stream;
}

QTextStream & operator<<(QTextStream & stream, const PluginDataList & plugins) {
	//Write a header with a "magic number" and a version
	stream << PLUGINLIST_HEADER_MAGIC << endl;
	stream << PLUGINLIST_HEADER_VERSION << endl;

	foreach (PluginData pluginData, plugins) {
		if (pluginData.fileName().isEmpty()) {
			//FIXME don't why, stream can contain empty datas
			//even if we didn't put empty datas in it (!)
			qCritical() << __FUNCTION__ << "Error: empty pluginData:" << pluginData.uuid();
		} else {
			stream << pluginData.fileName() << endl;
			stream << pluginData.uuid() << endl;
			stream << pluginData.isEnabled() << endl;
		}
	}
	return stream;
}

QTextStream & operator>>(QTextStream & stream, PluginDataList & plugins) {
	if (stream.atEnd()) {
		//Empty stream
		return stream;
	}

	//Read and check the header
	quint32 magic;
	stream >> magic;
	if (magic != PLUGINLIST_HEADER_MAGIC) {
		qCritical() << __FUNCTION__ << "Error: wrong magic number:" << magic;
		return stream;
	}

	//Read the version
	quint32 version;
	stream >> version;
	if (version != PLUGINLIST_HEADER_VERSION) {
		qCritical() << __FUNCTION__ << "Error: wrong version number:" << version;
		return stream;
	}

	while (!stream.atEnd()) {
		//FIXME this is buggy if the filename contains whitespaces
		//I don't know how to make QTextStream aware about string spaces :/
		//See http://doc.trolltech.com/main-snapshot/qtextstream.html#operator-gt-gt-4
		//Anyway, plugin filenames should not contain whitespaces
		QString filename;
		stream >> filename;
		///

		QString uuid;
		int enabled;
		stream >> uuid;
		stream >> enabled;

		if (filename.isEmpty()) {
			//FIXME don't know why, stream can contain empty datas
			//even if we didn't put empty datas in it (!)
		} else {
			PluginData pluginData(filename, uuid, enabled);
			plugins.removeAll(pluginData);
			plugins += pluginData;
		}
	}
	return stream;
}
