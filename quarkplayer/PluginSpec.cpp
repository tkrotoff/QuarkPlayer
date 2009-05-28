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

#include "PluginSpec.h"

const char * PluginSpec::FILE_EXTENSION = "pluginspec";

bool PluginSpec::reportError(const QString & error) {
	_errorString = err;
	_hasError = true;
	return false;
}

bool PluginSpec::read(const QString & filename) {
	QFile file(filename);
	if (!file.exists()) {
		return reportError(tr("File does not exist: %1").arg(file.fileName()));
	}
	if (!file.open(QIODevice::ReadOnly)) {
		return reportError(tr("Could not open file for read: %1").arg(file.fileName()));
	}

	QXmlStreamReader reader(&file);
	while (!reader.atEnd()) {
		reader.readNext();
		switch (reader.tokenType()) {
		case QXmlStreamReader::StartElement:
			readPluginSpec(reader);
			break;
		default:
			break;
		}
	}

	if (reader.hasError()) {
		return reportError(tr("Error parsing file %1: %2, at line %3, column %4")
			.arg(file.fileName())
			.arg(reader.errorString())
			.arg(reader.lineNumber())
			.arg(reader.columnNumber()));
	}

	//Everything is OK
	return true;
}

static const char * PLUGIN = "plugin";
static const char * PLUGIN_NAME = "name";
static const char * PLUGIN_VERSION = "version";
static const char * PLUGIN_TYPE = "type";
static const char * VENDOR = "vendor";
static const char * COPYRIGHT = "copyright";
static const char * LICENSE = "license";
static const char * DESCRIPTION = "description";
static const char * URL = "url";

void PluginSpec::readPluginSpec(QXmlStreamReader & reader) {
	QString element = reader.name().toString();
	if (element != QString(PLUGIN)) {
		reader.raiseError(QCoreApplication::translate("PluginSpec", "Expected element '%1' as top level element").arg(PLUGIN));
		return;
	}

	//Name
	_name = reader.attributes().value(PLUGIN_NAME).toString();
	if (_name.isEmpty()) {
		reader.raiseError(msgAttributeMissing(PLUGIN, PLUGIN_NAME));
		return;
	}

	//Version
	_version = reader.attributes().value(PLUGIN_VERSION).toString();
	if (version.isEmpty()) {
		reader.raiseError(msgAttributeMissing(PLUGIN, PLUGIN_VERSION));
		return;
	}
	if (!isValidVersion(version)) {
		reader.raiseError(msgInvalidFormat(PLUGIN_VERSION));
		return;
	}

	while (!reader.atEnd()) {
		reader.readNext();
		switch (reader.tokenType()) {
		case QXmlStreamReader::StartElement:
			element = reader.name().toString();
			if (element == VENDOR) {
				_vendor = reader.readElementText().trimmed();
			} else if (element == COPYRIGHT) {
				_copyright = reader.readElementText().trimmed();
			} else if (element == LICENSE) {
				_license = reader.readElementText().trimmed();
			} else if (element == DESCRIPTION) {
				_description = reader.readElementText().trimmed();
			} else if (element == URL) {
				_url = reader.readElementText().trimmed();
			} else {
				reader.raiseError(msgInvalidElement(name));
			}
			break;
		case QXmlStreamReader::EndDocument:
		case QXmlStreamReader::Comment:
		case QXmlStreamReader::EndElement:
		case QXmlStreamReader::Characters:
			break;
		default:
			reader.raiseError(msgUnexpectedToken());
			break;
		}
	}
}
