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

#include "ShortcutsFileParser.h"

#include "ShortcutConfigWidget.h"

#include <QtGui/QAction>

#include <QtCore/QFile>

#include <QtXml/QDomDocument>

ShortcutsFileParser::ShortcutsFileParser(const QString & fileName)
	: _fileName(fileName) {
}

ShortcutsFileParser::~ShortcutsFileParser() {
}

QMap<QString, QKeySequence> ShortcutsFileParser::load() const {
	QMap<QString, QKeySequence> result;

	QFile file(_fileName);
	if (!file.open(QIODevice::ReadOnly)) {
		return result;
	}

	QDomDocument doc("KeyboardMappingScheme");
	if (!doc.setContent(&file)) {
		return result;
	}

	QDomElement root = doc.documentElement();
	if (root.nodeName() != QLatin1String("mapping")) {
		return result;
	}

	QDomElement ks = root.firstChildElement();
	for (; !ks.isNull(); ks = ks.nextSiblingElement()) {
		if (ks.nodeName() == QLatin1String("shortcut")) {
			QString id = ks.attribute(QLatin1String("id"));
			QKeySequence shortcutKey;
			QDomElement keyElem = ks.firstChildElement("key");
			if (!keyElem.isNull()) {
				shortcutKey = QKeySequence(keyElem.attribute("value"));
			}
			result.insert(id, shortcutKey);
		}
	}

	file.close();
	return result;
}

bool ShortcutsFileParser::save(const QList<ShortcutItem *> & shortcutItems) {
	QFile file(_fileName);
	if (!file.open(QIODevice::WriteOnly)) {
		return false;
	}

	QDomDocument doc("KeyboardMappingScheme");
	QDomElement root = doc.createElement("mapping");
	doc.appendChild(root);

	for (int i = 0; i < shortcutItems.count(); ++i) {
		ShortcutItem * shortcutItem = shortcutItems.at(i);
		QDomElement ctag = doc.createElement("shortcut");
		ctag.setAttribute(QLatin1String("id"), shortcutItem->action->objectName());
		root.appendChild(ctag);

		QDomElement ktag = doc.createElement("key");
		ktag.setAttribute(QLatin1String("value"), ShortcutConfigWidget::toString(shortcutItem->shortcuts));
		ctag.appendChild(ktag);
	}

	file.write(doc.toByteArray());
	file.close();
	return true;
}
