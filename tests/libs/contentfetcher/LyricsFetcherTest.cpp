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

#include "LyricsFetcherTest.h"

#include <contentfetcher/LyricsFetcher.h>

void LyricsFetcherTest::initTestCase() {
	_lyricsFetcher = new LyricsFetcher(this);
	connect(_lyricsFetcher,
		SIGNAL(finished(QNetworkReply::NetworkError, const QUrl &, const QByteArray &, const ContentFetcherTrack &)),
		SLOT(lyricsFound(QNetworkReply::NetworkError, const QUrl &, const QByteArray &, const ContentFetcherTrack &))
	);
}

void LyricsFetcherTest::cleanupTestCase() {
	delete _lyricsFetcher;
}

void LyricsFetcherTest::fetch_data() {
	QTest::addColumn<QString>("artist");
	QTest::addColumn<QString>("title");

	QTest::addColumn<int>("lyricsError");
	QTest::addColumn<QString>("lyricsUrl");
	QTest::addColumn<QString>("lyricsContent");

	QTest::newRow("Michael Jackson") << "Michael Jackson" << "Don't Stop 'Til You Get Enough"
		<< static_cast<int>(QNetworkReply::NoError)
		<< "http://lyrics.wikia.com/Michael_Jackson:Don't_Stop_'Til_You_Get_Enough"
		<< "(spoken)<br />you know, I was, I was wondering if, <br />if you could keep on,<br />because the force, <br />it's got a lot of power,<br />and it make me feel like ah, <br />it make me feel like...    oooh!<br /><br />(1st Verse)<br />Lovely is the feelin' now<br />Fever, temperatures risin' now<br />Power (ah power) is the force the vow<br />That makes it happen<br />It asks no questions why (ooh)<br />So get closer (closer now) to my body now<br />Just love me 'til you don't know how (ooh) <br /><br />(Chorus)<br />Keep on with the force, don't stop<br />Don't stop 'til you get enough<br />Keep on with the force, don't stop<br />Don't stop 'til you get enough<br />Keep on with the force don't stop<br />Don't stop 'til you get enough<br />Keep on with the force don't stop<br />Don't stop 'til you get enough<br /><br />(2nd Verse)<br />Touch me and I feel on fire<br />Ain't nothin' like a love desire (ooh)<br />I'm melting (I'm melting) like hot candle wax<br />Sensation (ah sensation) lovely where we at (ooh)<br />So let love take us through the hours<br />I won't be complanin'<br />'Cause this is love power (ooh)<br /><br />(Chorus)<br />Keep on with the force don't stop<br />Don't stop 'til you get enough<br />Keep on with the force don't stop<br />Don't stop 'til you get enough<br />Keep on with the force don't stop<br />Don't stop 'til you get enough<br />Keep on with the force don't stop<br />Don't stop 'til you get enough<br />(Ooh)<br /><br />(3rd Verse)<br />Heartbreak enemy despise<br />Eternal (ah eternal) love shines in my eyes (ooh)<br />So let love (oh let love) take us through the hours<br />I won't be complanin' (no no)<br />'Cause your love is alright, alright<br /><br />(Chorus)<br />Keep on with the force don't stop<br />Don't stop 'til you get enough<br />Keep on with the force don't stop<br />Don't stop 'til you get enough<br />Keep on with the force don't stop<br />Don't stop 'til you get enough<br />Keep on with the force don't stop<br />Don't stop 'til you get enough<br />Keep on with the force don't stop<br />Don't stop 'til you get enough<br />Keep on with the force don't stop<br />Don't stop 'til you get enough<br />Keep on with the force don't stop<br />Don't stop 'til you get enough<br />Keep on with the force don't stop<br />Don't stop 'til you get enough <br /><br />(Refrain)<br />Lovely is the feeling now<br />I won't be complanin' (ooh ooh)<br />The force is love power<br /><br />(Chorus)<br />Keep on with the force don't stop<br />Don't stop 'til you get enough<br />Keep on with the force don't stop<br />Don't stop 'til you get enough<br />Keep on with the force don't stop<br />Don't stop 'til you get enough<br />Keep on with the force don't stop<br />Don't stop 'til you get enough<br />Keep on with the force don't stop<br />Don't stop 'til you get enough<br />Keep on with the force don't stop<br />Don't stop 'til you get enough<br />Keep on with the force don't stop<br />Don't stop 'til you get enough<br />Keep on with the force don't stop<br />Don't stop 'til you get enough <br />(ad-lib to fade)";

	//QTest::newRow("Noir D�sir") << "Noir D�sir" << "L'Homme Press�"
	//	<< static_cast<int>(QNetworkReply::NoError)
	//	<< "http://lyrics.wikia.com/Noir_D�sir:L'homme_Press�"
	//	<< "J'suis un mannequin glac� avec un teint de soleil<br />Raval�, l'homme press�, mes conneries prof�r�es<br />Sont le destin du monde, je n'ai pas le temps, je file<br />Ma carri�re est en jeu, je suis l'homme m�diatique<br />Je suis plus que politique, je vais vite, tr�s vite<br />J'suis une com�te humaine universelle<br /><br />Je traverse le temps, je suis une r�f�rence<br />Je suis omnipr�sent, je deviens omniscient<br />J'ai envahi le monde que je ne connais pas<br />Peu importe j'en parle, peu importe je sais<br />J'ai les hommes � mes pieds, huit milliards potentiels<br />De cr�tins asservis � part certains de mes amis<br />Du m�me monde que moi<br />Vous n'imaginez pas ce qu'ils sont gais<br /><br />Qui veut de moi et des miettes de mon cerveau?<br />Qui veut entrer dans la toile de mon r�seau?<br /><br />Militant quotidien de l'inhumanit�<br />Des profits imm�diats, des faveurs des m�dias<br />Moi je suis riche, tr�s riche<br />J'fais dans l'immobilier<br />Je sais faire des affaires, y'en a qui peuvent payer<br />J'connais le tout Paris et puis le reste aussi<br />Mes connaissances uniques et leurs femmes que je...fr�quente �videmment<br /><br />Les cordons de la bourse se rel�chent pour moi<br />Il n'y a plus de secrets, je suis le roi des rois<br />Explos� l'audimat, pulv�ris�e l'audience<br />Et qu'est-ce que vous croyez? C'est ma voie, c'est ma chance<br />J'adore les �missions � la t�l�vision<br />Pas le temps d'regarder mais c'est moi qui les fais<br />On crache la nourriture � ces yeux affam�s<br />Vous voyez qu'ils demandent, nous les savons avides<br />De notre pourriture. Mieux que d'la confiture<br />A des cochons!<br /><br />Qui veut de moi et des miettes de mon cerveau?<br />Qui veut entrer dans la toile de mon r�seau?<br /><br />Vous savez que je suis...<br />Un homme press�<br />Un homme press�<br />Un homme press�<br />Je suis...<br />Un homme press�<br />Un homme press�<br />Un homme press�<br /><br />Je suis un militant quotidien de l'inhumanit�<br />Et des profits imm�diats et puis des faveurs des m�dias<br />Moi je suis riche, tr�s riche<br />J'fais dans l'immobilier<br />Je sais faire des affaires y'en a qui peuvent payer<br />Et puis je traverse le temps, je suis devenu omnipr�sent<br />Je suis une super r�f�rence, je peux toujours ramener ma science<br />Moi je vais vite, tr�s vite, ma carri�re est en jeu<br />Je suis l'homme m�diatique, moi je suis plus que politique<br />Car je suis...<br />Un homme press�<br />Un homme press�<br />Un homme press�<br />Un homme press� (Oui je suis)<br />Un homme press�<br />Un homme press�<br /><br />Je suis un militant quotidien de l'inhumanit�<br />Et des profits imm�diats et puis des faveurs des m�dias<br />Moi je suis riche, tr�s riche<br />Je fais dans l'immobilier<br />Je sais faire des affaires y'en a qui peuvent payer<br /><br />Love, Love, Love, dit-on en Am�rique<br />Lioubov, Russie ex-Sovi�tique<br />Amour, aux quatre coins de France";

	QTest::newRow("Non existing") << "Non existing artist" << "Non existing title"
		<< static_cast<int>(QNetworkReply::ContentNotFoundError)
		<< "http://lyrics.wikia.com/api.php?action=lyrics&func=getSong&fmt=xml&artist=Non existing artist&song=Non existing title"
		<< "";

	QTest::newRow("Empty") << "" << ""
		<< static_cast<int>(QNetworkReply::ContentNotFoundError)
		<< "http://lyrics.wikia.com/Main_Page"
		<< "";
}

void LyricsFetcherTest::fetch() {
	QSignalSpy spyFinished(_lyricsFetcher, SIGNAL(finished(QNetworkReply::NetworkError,
				const QUrl &, const QByteArray &, const ContentFetcherTrack &)));

	QFETCH(QString, artist);
	QFETCH(QString, title);

	ContentFetcherTrack track;
	track.artist = artist;
	track.title = title;
	_lyricsFetcher->start(track);
	QTestEventLoop::instance().enterLoop(30);
	QVERIFY(!QTestEventLoop::instance().timeout());

	QCOMPARE(spyFinished.count(), 1);
}

void LyricsFetcherTest::lyricsFound(QNetworkReply::NetworkError error, const QUrl & url, const QByteArray & lyrics, const ContentFetcherTrack & track) {
	QFETCH(QString, artist);
	QCOMPARE(artist, track.artist);

	QFETCH(QString, title);
	QCOMPARE(title, track.title);

	QFETCH(int, lyricsError);
	QCOMPARE(lyricsError, static_cast<int>(error));

	QFETCH(QString, lyricsUrl);
	QCOMPARE(lyricsUrl, url.toString());

	QFETCH(QString, lyricsContent);
	QCOMPARE(lyricsContent, QString::fromUtf8(lyrics));

	QTestEventLoop::instance().exitLoop();
}
