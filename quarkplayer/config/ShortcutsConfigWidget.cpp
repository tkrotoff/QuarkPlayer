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

#include "ShortcutsConfigWidget.h"

#include "ui_ShortcutsConfigWidget.h"

#include "ShortcutsFileParser.h"

#include "Config.h"

#include <tkutil/TkIcon.h>
#include <tkutil/TkAction.h>
#include <tkutil/ActionCollection.h>
#include <tkutil/TkFileDialog.h>

#include <QtGui/QtGui>

#include <QtCore/QDebug>

Q_DECLARE_METATYPE(ShortcutItem *);

static const int COLUMN_ACTION = 0;
static const int COLUMN_LABEL = 1;
static const int COLUMN_SHORTCUT = 2;

ShortcutsConfigWidget::ShortcutsConfigWidget() {
	_keyNum = _key[0] = _key[1] = _key[2] = _key[3] = 0;

	_ui = new Ui::ShortcutsConfigWidget();
	_ui->setupUi(this);

	connect(_ui->resetButton, SIGNAL(clicked()), SLOT(resetShortcuts()));
	connect(_ui->removeButton, SIGNAL(clicked()), SLOT(removeShortcuts()));
	connect(_ui->saveButton, SIGNAL(clicked()), SLOT(saveShortcuts()));
	connect(_ui->loadButton, SIGNAL(clicked()), SLOT(loadShortcuts()));
	connect(_ui->defaultButton, SIGNAL(clicked()), SLOT(revertShortcutsToDefault()));

	_ui->actionList->sortByColumn(COLUMN_ACTION, Qt::AscendingOrder);

	connect(_ui->searchLineEdit, SIGNAL(textChanged(const QString &)), SLOT(searchChanged(const QString &)));
	connect(_ui->actionList, SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)),
		SLOT(actionChanged(QTreeWidgetItem *)));

	_ui->shortcutEdit->installEventFilter(this);
	connect(_ui->shortcutEdit, SIGNAL(textChanged(QString)), SLOT(shortcutChanged()));

	QHeaderView * header = _ui->actionList->header();
	header->resizeSection(COLUMN_ACTION, 210);
	header->resizeSection(COLUMN_LABEL, 110);
	header->setStretchLastSection(true);

	actionChanged(NULL);

	retranslate();
}

ShortcutsConfigWidget::~ShortcutsConfigWidget() {
	delete _ui;

	qDeleteAll(_shortcutItems);
	_shortcutItems.clear();
	_ui->actionList->clear();
}

QString ShortcutsConfigWidget::name() const {
	return tr("Shortcuts");
}

QString ShortcutsConfigWidget::iconName() const {
	return "preferences-desktop-keyboard";
}

void ShortcutsConfigWidget::saveConfig() {
	foreach (ShortcutItem * shortcutItem, _shortcutItems) {
		shortcutItem->action->setShortcuts(shortcutItem->shortcuts);
	}

	qDeleteAll(_shortcutItems);
	_shortcutItems.clear();
	_ui->actionList->clear();
}

void ShortcutsConfigWidget::readConfig() {
	qDeleteAll(_shortcutItems);
	_shortcutItems.clear();
	_ui->actionList->clear();

	QList<QAction *> actions = ActionCollection::actions();

	foreach (QAction * action, actions) {
		TkAction * tkAction = qobject_cast<TkAction *>(action);
		//If action is not a TkAction then we don't add it to the list of actions to be
		//displayed and configurable
		if (tkAction) {
			QTreeWidgetItem * item = new QTreeWidgetItem(_ui->actionList);

			QString name = tkAction->objectName();

			ShortcutItem * shortcutItem = new ShortcutItem();
			shortcutItem->action = tkAction;
			shortcutItem->shortcuts = tkAction->shortcuts();
			shortcutItem->treeItem = item;
			_shortcutItems << shortcutItem;

			QIcon icon = tkAction->icon();
			if (icon.isNull()) {
				icon = QIcon(":/icons/hi16-emptyicon.png");
			}
			item->setIcon(COLUMN_ACTION, icon);
			item->setText(COLUMN_ACTION, name);
			item->setText(COLUMN_LABEL, tkAction->toolTip());
			item->setText(COLUMN_SHORTCUT, toString(shortcutItem->shortcuts));
			item->setData(COLUMN_ACTION, Qt::UserRole, qVariantFromValue(shortcutItem));
		}
	}
}

void ShortcutsConfigWidget::retranslate() {
	_ui->retranslateUi(this);

	_ui->resetButton->setIcon(TkIcon("edit-undo"));
	_ui->removeButton->setIcon(TkIcon("edit-delete"));

	_ui->searchLineEdit->clearButton()->setToolTip(tr("Clear Search"));
	_ui->searchLineEdit->clearButton()->setIcon(TkIcon("edit-clear-locationbar-rtl"));

	_ui->searchLineEdit->showWordListButton()->setToolTip(tr("Previous Searches"));
	_ui->searchLineEdit->showWordListButton()->setIcon(TkIcon("go-down-search"));

	_ui->searchLineEdit->setToolTip(tr("Search shortcuts, use whitespaces to separate words"));
	_ui->searchLineEdit->setClickMessage(tr("Enter search terms here"));
}

bool ShortcutsConfigWidget::eventFilter(QObject * object, QEvent * event) {
	Q_UNUSED(object);

	if (event->type() == QEvent::KeyPress) {
		QKeyEvent * key = static_cast<QKeyEvent *>(event);
		handleKeyEvent(key);
		return true;
	}

	if (event->type() == QEvent::Shortcut ||
		event->type() == QEvent::ShortcutOverride  ||
		event->type() == QEvent::KeyRelease) {
		return true;
	}

	return false;
}

void ShortcutsConfigWidget::actionChanged(QTreeWidgetItem * currentItem) {
	if (!currentItem || !currentItem->data(COLUMN_ACTION, Qt::UserRole).isValid()) {
		_ui->shortcutEdit->setText(QString());
		_ui->keyGroupBox->setEnabled(false);
		return;
	}
	_ui->keyGroupBox->setEnabled(true);
	ShortcutItem * shortcutItem = qVariantValue<ShortcutItem *>(currentItem->data(COLUMN_ACTION, Qt::UserRole));
	setShortcuts(shortcutItem->shortcuts);
}

void ShortcutsConfigWidget::searchChanged(const QString & pattern) {
	for (int i = 0; i < _ui->actionList->topLevelItemCount(); ++i) {
		QTreeWidgetItem * item = _ui->actionList->topLevelItem(i);
		item->setHidden(search(pattern, item));
	}
}

void ShortcutsConfigWidget::shortcutChanged() {
	QTreeWidgetItem * currentItem = _ui->actionList->currentItem();
	if (currentItem && currentItem->data(COLUMN_ACTION, Qt::UserRole).isValid()) {
		ShortcutItem * shortcutItem = qVariantValue<ShortcutItem *>(currentItem->data(COLUMN_ACTION, Qt::UserRole));
		QList<QKeySequence> shortcuts;
		shortcuts += _key[0];
		shortcuts += _key[1];
		shortcuts += _key[2];
		shortcuts += _key[3];
		shortcutItem->shortcuts = shortcuts;
		currentItem->setText(COLUMN_SHORTCUT, toString(shortcutItem->shortcuts));
	}
}

void ShortcutsConfigWidget::setShortcuts(const QList<QKeySequence> & shortcuts) {
	_keyNum = _key[0] = _key[1] = _key[2] = _key[3] = 0;
	_keyNum = shortcuts.count();
	for (int i = 0; i < _keyNum; ++i) {
		_key[i] = shortcuts[i];
	}
	_ui->shortcutEdit->setText(toString(shortcuts));
}

bool ShortcutsConfigWidget::search(const QString & pattern, const QTreeWidgetItem * item) {
	if (item->childCount() == 0) {
		if (pattern.isEmpty()) {
			return false;
		}
		for (int i = 0; i < item->columnCount(); ++i) {
			if (item->text(i).contains(pattern, Qt::CaseInsensitive)) {
				return false;
			}
		}
		return true;
	}

	bool found = false;
	for (int i = 0; i < item->childCount(); ++i) {
		QTreeWidgetItem * citem = item->child(i);
		if (search(pattern, citem)) {
			citem->setHidden(true);
		} else {
			citem->setHidden(false);
			found = true;
		}
	}
	return !found;
}

void ShortcutsConfigWidget::resetShortcuts() {
	QTreeWidgetItem * currentItem = _ui->actionList->currentItem();
	if (currentItem && currentItem->data(COLUMN_ACTION, Qt::UserRole).isValid()) {
		ShortcutItem * shortcutItem = qVariantValue<ShortcutItem *>(currentItem->data(COLUMN_ACTION, Qt::UserRole));
		TkAction * action = qobject_cast<TkAction *>(shortcutItem->action);
		if (action) {
			setShortcuts(action->defaultShortcuts());
		}
	}
}

void ShortcutsConfigWidget::removeShortcuts() {
	_keyNum = _key[0] = _key[1] = _key[2] = _key[3] = 0;
	_ui->shortcutEdit->clear();
}

void ShortcutsConfigWidget::loadShortcuts() {
	QString fileName = TkFileDialog::getOpenFileName(this, tr("Load Keyboard Mapping Scheme"),
		Config::instance().resourceDir() + "/schemes/",
		tr("Keyboard Mapping Scheme (*.kms)")
	);

	if (!fileName.isEmpty()) {
		ShortcutsFileParser parser(fileName);
		QMap<QString, QKeySequence> mapping = parser.load();

		foreach (ShortcutItem * shortcutItem, _shortcutItems) {
			QString id = shortcutItem->action->objectName();
			if (mapping.contains(id)) {
				shortcutItem->shortcuts = fromString(mapping.value(id));
				shortcutItem->treeItem->setText(COLUMN_SHORTCUT, toString(shortcutItem->shortcuts));
				if (shortcutItem->treeItem == _ui->actionList->currentItem()) {
					actionChanged(shortcutItem->treeItem);
				}
			}
		}
	}
}

void ShortcutsConfigWidget::saveShortcuts() {
	QString fileName = TkFileDialog::getSaveFileName(this, tr("Save Keyboard Mapping Scheme"),
		Config::instance().resourceDir() + "/schemes/",
		tr("Keyboard Mapping Scheme (*.kms)")
	);

	if (!fileName.isEmpty()) {
		ShortcutsFileParser parser(fileName);
		parser.save(_shortcutItems);
	}
}

void ShortcutsConfigWidget::revertShortcutsToDefault() {
	foreach (ShortcutItem * shortcutItem, _shortcutItems) {
		TkAction * action = qobject_cast<TkAction *>(shortcutItem->action);
		if (action) {
			shortcutItem->shortcuts = action->defaultShortcuts();
		}
		shortcutItem->treeItem->setText(COLUMN_SHORTCUT, toString(shortcutItem->shortcuts));
		if (shortcutItem->treeItem == _ui->actionList->currentItem()) {
			actionChanged(shortcutItem->treeItem);
		}
	}
}

void ShortcutsConfigWidget::handleKeyEvent(QKeyEvent * event) {
	int nextKey = event->key();
	if (_keyNum > 3 ||
		nextKey == Qt::Key_Control ||
		nextKey == Qt::Key_Shift ||
		nextKey == Qt::Key_Meta ||
		nextKey == Qt::Key_Alt) {
		return;
	}

	nextKey |= translateModifiers(event->modifiers(), event->text());
	switch (_keyNum) {
	case 0:
		_key[0] = nextKey;
		break;
	case 1:
		_key[1] = nextKey;
		break;
	case 2:
		_key[2] = nextKey;
		break;
	case 3:
		_key[3] = nextKey;
		break;
	default:
		break;
	}
	_keyNum++;

	QList<QKeySequence> shortcuts;
	shortcuts += _key[0];
	shortcuts += _key[1];
	shortcuts += _key[2];
	shortcuts += _key[3];
	_ui->shortcutEdit->setText(toString(shortcuts));

	event->accept();
}

int ShortcutsConfigWidget::translateModifiers(Qt::KeyboardModifiers state, const QString & text) {
	int result = 0;

	//The shift modifier only counts when it is not used to type a symbol
	//that is only reachable using the shift key anyway
	if ((state & Qt::ShiftModifier) &&
		(text.size() == 0 ||
		!text.at(0).isPrint() ||
		text.at(0).isLetter() ||
		text.at(0).isSpace())) {
		result |= Qt::SHIFT;
	}
	if (state & Qt::ControlModifier) {
		result |= Qt::CTRL;
	}
	if (state & Qt::MetaModifier) {
		result |= Qt::META;
	}
	if (state & Qt::AltModifier) {
		result |= Qt::ALT;
	}

	return result;
}

QString ShortcutsConfigWidget::toString(const QList<QKeySequence> & shortcuts) {
	QStringList strList;
	foreach (QKeySequence shortcut, shortcuts) {
		QString tmp = shortcut;
		if (!tmp.isEmpty()) {
			strList += shortcut;
		}
	}
	return strList.join(", ");
}

QList<QKeySequence> ShortcutsConfigWidget::fromString(const QString & shortcuts) {
	QList<QKeySequence> shortcutList;
	QStringList strList = shortcuts.split(", ");
	foreach (QString str, strList) {
		if (!str.isEmpty()) {
			shortcutList += QKeySequence::fromString(str);
		}
	}
	return shortcutList;
}
