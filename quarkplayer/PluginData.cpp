/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008  Tanguy Krotoff <tkrotoff@gmail.com>
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

#include <QtCore/QStringList>
#include <QtCore/QPluginLoader>

PluginData::PluginData(const QUuid & uuid, bool enabled) {
	_uuid = uuid;
	_enabled = enabled;

	_loader = NULL;
	_interface = NULL;
}

PluginData::PluginData(const PluginData & pluginData) {
	copy(pluginData);
}

PluginData::PluginData(const QString & data) {
	QStringList tmp = data.split(";");
	_uuid = QUuid(tmp[0]);
	_enabled = tmp[1].toInt();

	_loader = NULL;
	_interface = NULL;
}

PluginData::PluginData() {
	_enabled = false;

	_loader = NULL;
	_interface = NULL;
}

PluginData::~PluginData() {
}

void PluginData::copy(const PluginData & pluginData) {
	_uuid = pluginData._uuid;
	_enabled = pluginData._enabled;
	_loader = pluginData._loader;
	_interface = pluginData._interface;
}

PluginData & PluginData::operator=(const PluginData & right) {
	//Handle self-assignment
	if (this == &right) {
		return *this;
	}

	copy(right);
	return *this;
}

int PluginData::operator==(const PluginData & right) {
	return _uuid == right._uuid;
}

QString PluginData::toString() const {
	QStringList tmp;
	tmp << _uuid.toString();
	tmp << QString::number(_enabled);
	return tmp.join(";");
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

QPluginLoader * PluginData::loader() const {
	return _loader;
}

void PluginData::setLoader(QPluginLoader * loader) {
	_loader = loader;
}

void PluginData::deleteLoader() {
	//This is too dangerous: it crashes
	if (_loader) {
		_loader->unload();
		delete _loader;
		_loader = NULL;
	}
}

PluginInterface * PluginData::interface() const {
	return _interface;
}

void PluginData::setInterface(PluginInterface * interface) {
	_interface = interface;
}

void PluginData::deleteInterface() {
	//FIXME this does not work, it crashes using MSVC80
	delete _interface;
	_interface = NULL;
}
