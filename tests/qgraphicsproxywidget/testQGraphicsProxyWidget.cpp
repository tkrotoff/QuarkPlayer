/*
 * Copyright (C) 2009  Tanguy Krotoff <tkrotoff@gmail.com>
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

#include <phonon/mediaobject.h>
#include <phonon/videowidget.h>
#include <phonon/mediasource.h>
#include <phonon/audiooutput.h>

#include <QtGui/QtGui>

int main(int argc, char * argv[]) {
	QApplication app(argc, argv);
	app.setApplicationName("TestQGraphicsProxyWidget");
	app.setQuitOnLastWindowClosed(true);

	QStringList args(QCoreApplication::arguments());
	//if (args.count() < 2) {
	//	qDebug() << "Please provide a file to be played";
	//	return EXIT_SUCCESS;
	//}
	//QString fileName(args[1]);
	QString fileName("1ind-shrek3.sample.avi");

	QWidget * widget = new QWidget();

	Phonon::MediaObject * mediaObject = new Phonon::MediaObject();
	Phonon::VideoWidget * videoWidget = new Phonon::VideoWidget(widget);
	Phonon::AudioOutput * audioOutput = new Phonon::AudioOutput();
	Phonon::createPath(mediaObject, videoWidget);
	Phonon::createPath(mediaObject, audioOutput);

	QGraphicsScene scene;
	QGraphicsProxyWidget * proxy = scene.addWidget(widget);
	QGraphicsView view(&scene);
	view.show();

	mediaObject->setCurrentSource(fileName);
	mediaObject->play();

	return app.exec();
}
