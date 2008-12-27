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

#ifndef WINFILEASSOCIATIONSCONFIGWIDGET_H
#define WINFILEASSOCIATIONSCONFIGWIDGET_H

#include "IConfigWidget.h"

namespace Ui { class WinFileAssociationsConfigWidget; }

class WinFileAssociations;

class QTreeWidgetItem;

/**
 * Windows file associations configuration widget.
 *
 * @author Tanguy Krotoff
 */
class WinFileAssociationsConfigWidget : public IConfigWidget {
	Q_OBJECT
public:

	WinFileAssociationsConfigWidget();

	~WinFileAssociationsConfigWidget();

	QString name() const;

	QString iconName() const;

	void readConfig();

	void saveConfig();

	void retranslate();

private slots:

	void currentItemChanged(QTreeWidgetItem * item);

	void selectAllButtonClicked();

	void selectNoneButtonClicked();

private:

	void addItems(QTreeWidgetItem * parent, const QStringList & extensions);

	void selectButtonClicked(bool select);

	/**
	 * This is a hack in order to get the icons that matches an extension.
	 *
	 * Creates a temporary file with the proper extension and then use QFileIconProvider.
	 *
	 * @param extension file extension (mp3, divx...)
	 * @return icon associated with the file extension
	 * @see QFileIconProvider
	 */
	QIcon fileExtensionIcon(const QString & extension) const;

	Ui::WinFileAssociationsConfigWidget * _ui;

	WinFileAssociations * _winFileAssociations;
};

#endif	//WINFILEASSOCIATIONSCONFIGWIDGET_H
