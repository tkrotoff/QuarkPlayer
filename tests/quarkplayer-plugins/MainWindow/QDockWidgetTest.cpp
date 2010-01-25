/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2010  Tanguy Krotoff <tkrotoff@gmail.com>
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

#include "QDockWidgetTest.h"

#include <QtGui/QtGui>

#include <TkUtil/MouseEventFilter.h>

QTEST_MAIN(QDockWidgetTest)

void QDockWidgetTest::initTestCase() {
}

void QDockWidgetTest::cleanupTestCase() {
}

class DockWidget : public QDockWidget {
public:

	virtual QSize sizeHint() {
		return QSize(0, 0);
	}
};

void QDockWidgetTest::showGui() {
	_mainWindow = new QMainWindow();

	QDockWidget * dockWidget1 = new QDockWidget("dockWidget1");
	dockWidget1->setWidget(new QTextEdit("dockWidget1"));
	_mainWindow->addDockWidget(Qt::LeftDockWidgetArea, dockWidget1);

	QDockWidget * dockWidget2 = new QDockWidget("dockWidget2");
	dockWidget2->setWidget(new QTextEdit("dockWidget2"));
	_mainWindow->addDockWidget(Qt::LeftDockWidgetArea, dockWidget2);
	_mainWindow->tabifyDockWidget(dockWidget1, dockWidget2);

	/*QDockWidget **/ _dockWidget3 = new QDockWidget("dockWidget3");
	_dockWidget3->setWidget(new QTextEdit("dockWidget3"));
	_mainWindow->addDockWidget(Qt::RightDockWidgetArea, _dockWidget3);

	QDockWidget * dockWidget4 = new QDockWidget("dockWidget4");
	dockWidget4->setWidget(new QTextEdit("dockWidget4"));
	_mainWindow->addDockWidget(Qt::RightDockWidgetArea, dockWidget4);
	_mainWindow->tabifyDockWidget(_dockWidget3, dockWidget4);

	DockWidget * dockWidget5 = new DockWidget();
	dockWidget5->setWidget(new QTextEdit("dockWidget5"));
	_mainWindow->addDockWidget(Qt::RightDockWidgetArea, dockWidget5);
	//_mainWindow->tabifyDockWidget(NULL, dockWidget5);

	QToolButton * showQTabBarHackButton = new QToolButton();
	showQTabBarHackButton->setText("showQTabBarHack");
	connect(showQTabBarHackButton, SIGNAL(clicked()),
		SLOT(showQTabBarHack()));

	QToolBar * toolBar = new QToolBar();
	toolBar->addWidget(showQTabBarHackButton);
	_mainWindow->addToolBar(Qt::RightToolBarArea, toolBar);

	showQTabBarHack();

	_mainWindow->show();
	QApplication::exec();
}

bool PaintEventFilter::eventFilter(QObject * object, QEvent * event) {
	if (event->type() == QEvent::Paint) {
		return true;
	} else {
		//Standard event processing
		return QObject::eventFilter(object, event);
	}
}

void QDockWidgetTest::showQTabBarHack() {
	//Taken from http://vingrad.ru/blogs/sabrog/2008/12/26/qt-opredelyaem-tekuschiy-vidzhet-v-qtabbar-qdockwidgetov/
	//See also http://ariya.blogspot.com/2007/04/tab-bar-with-roundednorth-for-tabbed.html

	//recursively look like kids in QTabBar * QMainWindow
	QList<QTabBar *> lst = _mainWindow->findChildren<QTabBar *>();

	//255 TabBar'ov should suffice;)
	//quint8 i = 0;

	/*
	QTabBar'y can be created, but not destroy, so they can be invisible.
	There is an option when in TabBar'e only 2 DockWidget'a, with one floating (floating),
	the user see that TabBar disappeared but actually disappears only yarlychek.
	QTabBar disappears when a floating window be tied somewhere in QTabBare is only 1 element.
	In general, check with count'om optional to Detective
	where QTabBar'y have labels
	*/
	foreach (QTabBar * tab, lst) {
		if (tab->isVisible() /*&& tab->count() > 1*/) {
			//'re counting visible QTabBar'ov, if necessary
			//i++;

			/*
			Next is untranslatable play on words:)
			Qt developers in each yarlychek put a pointer to QDockWidget, which yarlychek belongs.
			Procedure for transfer pointer QWidget'a in QVariant user-defined type and vice versa.
			Last type quintptr - cross-platform version of the index, where it is the size of 32 bits, which do not need 64.
			*/
			quintptr wId = qvariant_cast<quintptr>(tab->tabData(tab->currentIndex()));

			QDockWidget * widget = reinterpret_cast<QDockWidget *>(wId);
			/*QMessageBox::information(
				_mainWindow, "Info",
				QString("class:%1, title:%2")
					.arg(widget->metaObject()->className())
					.arg(widget->windowTitle())
			);*/

			MousePressEventFilter * mousePressFilter = new MousePressEventFilter(this, SLOT(printHelloWorld()), Qt::LeftButton, false);
			tab->installEventFilter(mousePressFilter);

			PaintEventFilter * paintEventFilter = new PaintEventFilter();
			//tab->installEventFilter(paintEventFilter);

			//tab->removeTab(0);
			//tab->removeTab(1);
		}
	}
}

void QDockWidgetTest::printHelloWorld() {
	qDebug() << __FUNCTION__;
	_dockWidget3->widget()->resize(0, _dockWidget3->height());
}
