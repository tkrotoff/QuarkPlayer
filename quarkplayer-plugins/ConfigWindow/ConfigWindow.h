/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2011  Tanguy Krotoff <tkrotoff@gmail.com>
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

#ifndef CONFIGWINDOW_H
#define CONFIGWINDOW_H

#include <quarkplayer-plugins/ConfigWindow/ConfigWindowExport.h>

#include <QtGui/QDialog>

#include <QtCore/QList>
#include <QtCore/QMap>

class IConfigWidget;
class IPluginManager;
namespace Ui { class ConfigWindow; }

class QAbstractItemView;
class QTreeWidgetItem;

/**
 * QuarkPlayer configuration window.
 *
 * @author Tanguy Krotoff
 */
class CONFIGWINDOW_API ConfigWindow : public QDialog {
	Q_OBJECT
public:

	ConfigWindow(IPluginManager & pluginManager, QWidget * parent);

	~ConfigWindow();

	/**
	 * Adds an external ConfigWidget to the ConfigWindow.
	 *
	 * Useful for plugins that contain a ConfigWidget.
	 *
	 * @param configWidget ConfigWidget to add to the ConfigWindow
	 */
	void addConfigWidget(IConfigWidget * configWidget);

	void show();

	void exec();

private slots:

	void showConfigWidget(QTreeWidgetItem * item);

	void saveConfig();

	void retranslate();

private:

	void readConfig();

	void populateStackedWidget();

	static int computeListViewMinimumWidth(QAbstractItemView * view);

	Ui::ConfigWindow * _ui;

	/**
	 * The ConfigWindow contains ConfigWidgets.
	 */
	class ConfigWidget {
	public:

		ConfigWidget(IConfigWidget * _configWidget, bool _isPlugin = false) {
			configWidget = _configWidget;
			isPlugin = _isPlugin;
		}

		IConfigWidget * configWidget;

		/** If the IConfigWidget should be in the plugin category or not. */
		bool isPlugin;
	};

	/** List of all config widget. */
	QList<ConfigWidget> _configWidgetList;

	/**
	 * List of opened config widget.
	 *
	 * Trick for not having saving conflicts between config widgets.
	 * Saves the config widgets in the exact order they were last opened.
	 */
	QMap<IConfigWidget *, int> _configWidgetOpenedMap;

	int _lastConfigWindowOpenedIndex;
};

#endif	//CONFIGWINDOW_H
