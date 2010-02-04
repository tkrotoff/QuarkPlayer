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

#include "MediaInfoWindow.h"

#include "ui_MediaInfoWindow.h"

#include <MediaInfoFetcher/MediaInfoFetcher.h>
#include <MediaInfoFetcher/MetaDataWriter.h>

#include <ContentFetcher/LyricsFetcher.h>
#include <ContentFetcher/WikipediaArticle.h>

#include <WebBrowser/WebBrowser.h>

#include <TkUtil/LanguageChangeEventFilter.h>

#include <QtGui/QtGui>

#include <QtCore/QDebug>
#include <QtCore/QtGlobal>
#include <QtCore/QUrl>

MediaInfoWindow::MediaInfoWindow(QWidget * parent)
	: QDialog(parent) {

	_mediaInfoFetcher = NULL;
	_lyricsFetcher = NULL;
	_wikipediaArticle = NULL;

	_ui = new Ui::MediaInfoWindow();
	_ui->setupUi(this);

	//Web browser widget
	_webBrowser = new WebBrowser(_ui->artistTab);
	_ui->artistTab->layout()->setMargin(0);
	_ui->artistTab->layout()->setSpacing(0);
	_ui->artistTab->layout()->addWidget(_webBrowser);

	//Thumbnail view
	_ui->artworkTab->layout()->setMargin(0);
	_ui->artworkTab->layout()->setSpacing(0);

	//Refresh button
	_refreshButton = new QToolButton(this);
	_refreshButton->setAutoRaise(true);
	connect(_refreshButton, SIGNAL(clicked()), SLOT(refresh()));

	//Save metadata button
	connect(_ui->buttonBox, SIGNAL(accepted()), SLOT(writeMetaData()));

	//Open directory button
	_openDirectoryButton = new QToolButton(this);
	_openDirectoryButton->setAutoRaise(true);
	connect(_openDirectoryButton, SIGNAL(clicked()), SLOT(openDirectory()));

	//Tab corner widget
	QWidget * cornerWidget = new QWidget(this);
	QHBoxLayout * cornerWidgetLayout = new QHBoxLayout();
	cornerWidgetLayout->setMargin(0);
	cornerWidgetLayout->setSpacing(0);
	cornerWidgetLayout->addWidget(_refreshButton);
	cornerWidgetLayout->addWidget(_openDirectoryButton);
	cornerWidget->setLayout(cornerWidgetLayout);

	_ui->tabWidget->setCornerWidget(cornerWidget, Qt::TopRightCorner);

	RETRANSLATE(this);
	retranslate();
}

MediaInfoWindow::~MediaInfoWindow() {
}

void MediaInfoWindow::setMediaInfoFetcher(MediaInfoFetcher * mediaInfoFetcher) {
	//Disconnect MediaInfoWindow from the previous MediaInfoFetcher if any
	if (_mediaInfoFetcher) {
		disconnect(_mediaInfoFetcher, SIGNAL(finished(const MediaInfo &)),
			this, SLOT(updateMediaInfo(const MediaInfo &)));
	}

	//New MediaInfoFetcher
	_mediaInfoFetcher = mediaInfoFetcher;
	connect(_mediaInfoFetcher, SIGNAL(finished(const MediaInfo &)),
		SLOT(updateMediaInfo(const MediaInfo &)));
}

void MediaInfoWindow::setLanguage(const QString & language) {
	_language = language;
}

void MediaInfoWindow::show() {
	QDialog::show();
	refresh();
}

void MediaInfoWindow::retranslate() {
	_refreshButton->setIcon(QIcon::fromTheme("view-refresh"));
	_refreshButton->setToolTip(tr("Refresh Informations"));

	_openDirectoryButton->setIcon(QIcon::fromTheme("document-open-folder"));
	_openDirectoryButton->setToolTip(tr("Open Directory"));

	_ui->retranslateUi(this);

	refresh();
}

void MediaInfoWindow::openDirectory() {
	QUrl url = QUrl::fromLocalFile(_ui->thumbnailView->lastRefreshedDirectory());
	QDesktopServices::openUrl(url);
}

void MediaInfoWindow::refresh() {
	if (_mediaInfoFetcher) {
		MediaInfo mediaInfo = _mediaInfoFetcher->mediaInfo();
		if (mediaInfo.fetched()) {
			updateMediaInfo(mediaInfo);
		}
	}
}

void MediaInfoWindow::writeMetaData() const {
	MetaDataWriter::write(_mediaInfoFetcher->mediaInfo());
}

void MediaInfoWindow::updateMediaInfo(const MediaInfo & mediaInfo) {
	bool isUrl = MediaInfo::isUrl(mediaInfo.fileName());
	_openDirectoryButton->setEnabled(isUrl);

	QIcon icon;
	if (!isUrl) {
		static QFileIconProvider iconProvider;
		icon = iconProvider.icon(QFileInfo(mediaInfo.fileName()));
	} else {
		icon = QIcon::fromTheme("document-open-remote");
	}
	_ui->fileTypeLabel->setPixmap(icon.pixmap(QSize(16, 16)));

	//General
	_ui->filenameLineEdit->setText(mediaInfo.fileName());

	//Metadata
	_ui->trackLineEdit->setText(QString::number(mediaInfo.metaDataValue(MediaInfo::TrackNumber).toInt()));
	_ui->discLineEdit->setText(QString::number(mediaInfo.metaDataValue(MediaInfo::DiscNumber).toInt()));
	_ui->titleLineEdit->setText(mediaInfo.metaDataValue(MediaInfo::Title).toString());
	_ui->artistLineEdit->setText(mediaInfo.metaDataValue(MediaInfo::Artist).toString());
	_ui->albumLineEdit->setText(mediaInfo.metaDataValue(MediaInfo::Album).toString());
	_ui->albumArtistLineEdit->setText(mediaInfo.metaDataValue(MediaInfo::AlbumArtist).toString());
	_ui->originalArtistLineEdit->setText(mediaInfo.metaDataValue(MediaInfo::OriginalArtist).toString());
	_ui->yearLineEdit->setText(mediaInfo.metaDataValue(MediaInfo::Year).toDate().toString());
	_ui->genreLineEdit->setText(mediaInfo.metaDataValue(MediaInfo::Genre).toString());

	QString tmp = mediaInfo.metaDataValue(MediaInfo::Comment).toString();
	//" / " means new line inside a MP3 comment
	tmp.replace(" / ", "<br>");
	_ui->commentTextEdit->setHtml(tmp);

	_ui->composerLineEdit->setText(mediaInfo.metaDataValue(MediaInfo::Composer).toString());
	_ui->publisherLineEdit->setText(mediaInfo.metaDataValue(MediaInfo::Publisher).toString());
	_ui->copyrightLineEdit->setText(mediaInfo.metaDataValue(MediaInfo::Copyright).toString());
	_ui->urlLineEdit->setText(mediaInfo.metaDataValue(MediaInfo::URL).toString());
	_ui->bpmLineEdit->setText(QString::number(mediaInfo.metaDataValue(MediaInfo::BPM).toInt()));
	_ui->encodedByLineEdit->setText(mediaInfo.metaDataValue(MediaInfo::EncodedBy).toString());

	//MusicBrainz tags
	QString musicBrainzReleaseId = mediaInfo.metaDataValue(MediaInfo::MusicBrainzReleaseId).toString();
	QString musicBrainzArtistId = mediaInfo.metaDataValue(MediaInfo::MusicBrainzArtistId).toString();
	_ui->musicBrainzLinkLabel->clear();
	if (!musicBrainzReleaseId.isEmpty() && !musicBrainzArtistId.isEmpty()) {
		QString musicBrainz;
		musicBrainz += "<a href=\"http://musicbrainz.org/release/" + musicBrainzReleaseId + ".html\">Release Id</a>";
		musicBrainz += " / ";
		musicBrainz += "<a href=\"http://musicbrainz.org/artist/" + musicBrainzArtistId + ".html\">Artist Id</a>";
		_ui->musicBrainzLinkLabel->setText(musicBrainz);
	}
	///

	//Amazon ASIN
	QString amazonASIN = mediaInfo.metaDataValue(MediaInfo::AmazonASIN).toString();
	_ui->amazonLinkLabel->clear();
	if (!amazonASIN.isEmpty()) {
		QString websiteDomain = "com";
		if (_language == "en") { websiteDomain = "com"; }
		if (_language == "fr") { websiteDomain = "fr"; }
		if (_language == "de") { websiteDomain = "de"; }
		if (_language == "ja") { websiteDomain = "jp"; }
		if (_language == "zh") { websiteDomain = "cn"; }
		QString amazon;
		amazon += "<a href=\"http://www.amazon." + websiteDomain + "/gp/product/" + amazonASIN + "\">Amazon ASIN</a>";
		_ui->amazonLinkLabel->setText(amazon);
	}
	///

	static const QString br("<br>");

	//General
	QString fileInfo;
	int fileSize = mediaInfo.fileSize();
	QString duration(mediaInfo.durationFormatted());
	int bitrate = mediaInfo.bitrate();
	if (fileSize >= 0 || !duration.isEmpty() || bitrate >= 0) {
		QString fileTypeWikipedia(mediaInfo.fileType().wikipediaArticle);
		QString fileTypeName(mediaInfo.fileType().fullName);
		if (!fileTypeWikipedia.isEmpty()) {
			fileInfo += "<a href=\"http://" + _language + ".wikipedia.org/wiki/" + fileTypeWikipedia + "\">" + fileTypeName + "</a>";
		} else {
			fileInfo += fileTypeName;
		}
		if (fileSize >= 0) {
			if (!fileInfo.isEmpty()) {
				fileInfo += ", ";
			}
			fileInfo += QString::number(fileSize) + ' ' + tr("KB");
		}
		if (!duration.isEmpty()) {
			if (!fileInfo.isEmpty()) {
				fileInfo += ", ";
			}
			fileInfo += duration;
		}
		if (bitrate >= 0 && mediaInfo.videoStreamCount() > 0) {
			//Don't show the overall bitrate if there is no video stream
			//in this case the overall bitrate has no interest
			if (!fileInfo.isEmpty()) {
				fileInfo += ", ";
			}
			fileInfo += QString::number(bitrate) + ' ' + tr("kbps");
		}
	}
	QString encodedApplication;
	if (!mediaInfo.encodedApplication().isEmpty()) {
		encodedApplication = br + tr("Encoded Application:") + ' ' + mediaInfo.encodedApplication();
	}

	//Audio
	QString audioStream;
	int audioStreamCount = mediaInfo.audioStreamCount();
	for (int audioStreamId = 0; audioStreamId < audioStreamCount; audioStreamId++) {
		int audioBitrate = mediaInfo.audioStreamValue(audioStreamId, MediaInfo::AudioBitrate).toInt();
		QString audioBitrateMode = mediaInfo.audioStreamValue(audioStreamId, MediaInfo::AudioBitrateMode).toString();
		int audioSampleRate = mediaInfo.audioStreamValue(audioStreamId, MediaInfo::AudioSampleRate).toInt();
		int audioBitsPerSample = mediaInfo.audioStreamValue(audioStreamId, MediaInfo::AudioBitsPerSample).toInt();
		int audioChannelCount = mediaInfo.audioStreamValue(audioStreamId, MediaInfo::AudioChannelCount).toInt();
		QString audioCodecProfile = mediaInfo.audioStreamValue(audioStreamId, MediaInfo::AudioCodecProfile).toString();
		QString audioCodec = mediaInfo.audioStreamValue(audioStreamId, MediaInfo::AudioCodec).toString();
		QString audioLanguage = mediaInfo.audioStreamValue(audioStreamId, MediaInfo::AudioLanguage).toString();
		QString audioEncodedLibrary = mediaInfo.audioStreamValue(audioStreamId, MediaInfo::AudioEncodedLibrary).toString();

		if (audioBitrate > 0 || !audioBitrateMode.isEmpty() || audioSampleRate > 0 ||
			audioBitsPerSample > 0 || audioChannelCount > 0 ||
			!audioCodecProfile.isEmpty() || !audioCodec.isEmpty() ||
			!audioLanguage.isEmpty() || !audioEncodedLibrary.isEmpty()) {

			if (audioStreamCount > 1) {
				audioStream += br + tr("Audio Stream #%1:").arg(audioStreamId + 1);
			} else {
				audioStream += br + tr("Audio Stream:");
			}
			if (!audioLanguage.isEmpty()) {
				audioStream += ' ' + audioLanguage + ',';
			}
			if (audioBitrate > 0) {
				audioStream += QString(" %1 %2").arg(audioBitrate).arg(tr("kbps"));
			}
			if (!audioBitrateMode.isEmpty()) {
				audioStream += ' ' + audioBitrateMode;
			}
			if (audioSampleRate > 0) {
				audioStream += QString(", %1 %2").arg(audioSampleRate).arg(tr("kHz"));
			}
			if (audioBitsPerSample > 0) {
				audioStream += QString(", %1 %2").arg(audioBitsPerSample).arg(tr("bits"));
			}
			if (audioChannelCount > 0) {
				audioStream += QString(", %1 %2").arg(audioChannelCount).arg(tr("channels"));
			}
			if (!audioCodec.isEmpty()) {
				audioStream += ", " + audioCodec;
			}
			if (!audioCodecProfile.isEmpty()) {
				audioStream += ", " + audioCodecProfile;
			}
			if (!audioEncodedLibrary.isEmpty()) {
				audioStream += ", " + audioEncodedLibrary;
			}
		}
	}

	//Video
	QString videoStream;
	int videoStreamCount = mediaInfo.videoStreamCount();
	for (int videoStreamId = 0; videoStreamId < videoStreamCount; videoStreamId++) {
		int videoBitrate = mediaInfo.videoStreamValue(videoStreamId, MediaInfo::VideoBitrate).toInt();
		QSize videoResolution = mediaInfo.videoStreamValue(videoStreamId, MediaInfo::VideoResolution).toSize();
		int videoFrameRate = mediaInfo.videoStreamValue(videoStreamId, MediaInfo::VideoFrameRate).toInt();
		QString videoFormat = mediaInfo.videoStreamValue(videoStreamId, MediaInfo::VideoFormat).toString();
		QString videoCodec = mediaInfo.videoStreamValue(videoStreamId, MediaInfo::VideoCodec).toString();
		QString videoEncodedLibrary = mediaInfo.videoStreamValue(videoStreamId, MediaInfo::VideoEncodedLibrary).toString();

		if (videoBitrate > 0 || !videoResolution.isEmpty() ||
			videoFrameRate > 0 || !videoCodec.isEmpty() ||
			!videoEncodedLibrary.isEmpty()) {

			if (videoStreamCount > 1) {
				videoStream += br + tr("Video Stream #%1:").arg(videoStreamId + 1);
			} else {
				videoStream += br + tr("Video Stream:");
			}
			if (videoBitrate > 0) {
				videoStream += QString(" %1 %2").arg(videoBitrate).arg(tr("kbps"));
			}
			if (!videoResolution.isEmpty()) {
				videoStream += QString(", %1*%2")
					.arg(videoResolution.width()).arg(videoResolution.height());
			}
			if (videoFrameRate > 0) {
				videoStream += QString(", %1 %2").arg(videoFrameRate).arg(tr("FPS"));
			}
			if (!videoFormat.isEmpty()) {
				videoStream += ", " + videoFormat;
			}
			if (!videoCodec.isEmpty()) {
				videoStream += " (" + videoCodec + ')';
			}
			if (!videoEncodedLibrary.isEmpty()) {
				videoStream += ", " + videoEncodedLibrary;
			}
		}
	}

	//Text
	QString textStream;
	int textStreamCount = mediaInfo.textStreamCount();
	for (int textStreamId = 0; textStreamId < textStreamCount; textStreamId++) {
		QString textLanguage(mediaInfo.textStreamValue(textStreamId, MediaInfo::TextLanguage).toString());
		QString textFormat(mediaInfo.textStreamValue(textStreamId, MediaInfo::TextFormat).toString());

		if (!textFormat.isEmpty() || !textLanguage.isEmpty()) {
			if (textStreamCount > 1) {
				textStream += br + tr("Text Stream #%1:").arg(textStreamId + 1);
			} else {
				textStream += br + tr("Text Stream:");
			}
			if (!textLanguage.isEmpty()) {
				textStream += ' ' + textLanguage;
			}
			if (!textFormat.isEmpty()) {
				textStream += ", " + textFormat;
			}
		}
	}

	//Warning about proprietary codecs
	QString moreInfo;
	if (mediaInfo.fileType().name == FileType::WMA) {
		moreInfo += br + br + tr("Warning: \
			WMA is a proprietary, Windows specific audio codec that includes DRM and thus is not recommended. \
			Use a well supported standard audio codec instead like %1 or %2")
			.arg("<a href=\"http://" + _language + ".wikipedia.org/wiki/MP3\">MP3</a>")
			.arg("<a href=\"http://" + _language + ".wikipedia.org/wiki/FLAC\">FLAC</a>");
	} else if (mediaInfo.fileType().name == FileType::WMV) {
		moreInfo += br + br + tr("Warning: \
			WMV is a proprietary, Windows specific video codec that includes DRM and thus is not recommended. \
			Use a well supported standard video codec instead like %1 or %2")
			.arg("<a href=\"http://" + _language + ".wikipedia.org/wiki/Xvid\">Xvid</a>")
			.arg("<a href=\"http://" + _language + ".wikipedia.org/wiki/Theora\">Ogg/Theora</a>");
	}

	//Sets the label
	_ui->formatInfoLabel->setText(
		fileInfo + encodedApplication
		+ videoStream + audioStream + textStream
		+ moreInfo
	);

	//Refresh ThumbnailView
	QFileInfo filenameInfo(mediaInfo.fileName());
	if (filenameInfo.isDir()) {
		_ui->thumbnailView->setDir(filenameInfo.absoluteFilePath());
	} else {
		_ui->thumbnailView->setDir(filenameInfo.absolutePath());
	}
	_ui->thumbnailView->refresh();

	ContentFetcherTrack track;
	track.artist = mediaInfo.metaDataValue(MediaInfo::Artist).toString();
	track.title = mediaInfo.metaDataValue(MediaInfo::Title).toString();

	//Download the Wikipedia article
	_webBrowser->setHtml(tr("Looking for content..."));
	if (!_wikipediaArticle) {
		//Lazy initialization
		_wikipediaArticle = new WikipediaArticle(this);
		connect(_wikipediaArticle,
			SIGNAL(finished(QNetworkReply::NetworkError, const QUrl &, const QByteArray &, const ContentFetcherTrack &)),
			SLOT(wikipediaArticleFound(QNetworkReply::NetworkError, const QUrl &, const QByteArray &, const ContentFetcherTrack &))
		);
	}
	_wikipediaArticle->start(track, _language);
	///

	//Download the lyrics
	_ui->lyricsTextBrowser->setHtml(tr("Looking for content..."));
	if (!_lyricsFetcher) {
		//Lazy initialization
		_lyricsFetcher = new LyricsFetcher(this);
		connect(_lyricsFetcher,
			SIGNAL(finished(QNetworkReply::NetworkError, const QUrl &, const QByteArray &, const ContentFetcherTrack &)),
			SLOT(lyricsFound(QNetworkReply::NetworkError, const QUrl &, const QByteArray &, const ContentFetcherTrack &))
		);
	}
	_lyricsFetcher->start(track);
	///
}

void MediaInfoWindow::lyricsFound(QNetworkReply::NetworkError error, const QUrl & url, const QByteArray & lyrics, const ContentFetcherTrack & track) {
	Q_UNUSED(track);

	if (error == QNetworkReply::NoError) {
		QString text(QString::fromUtf8(lyrics));
		_ui->lyricsTextBrowser->setHtml(text
				+ "<br><br><a href src=" + url.toString() + ">" + url.toString() + "</a>");
	} else {
		_ui->lyricsTextBrowser->setHtml(tr("Error: ") + ContentFetcher::errorString(error)
				+ "<br><br><a href src=" + url.toString() + ">" + url.toString() + "</a>");
	}
}

void MediaInfoWindow::wikipediaArticleFound(QNetworkReply::NetworkError error, const QUrl & url, const QByteArray & wikipediaArticle, const ContentFetcherTrack & track) {
	Q_UNUSED(track);

	if (error == QNetworkReply::NoError) {
		_webBrowser->setUrlLineEdit(url.toString());
		_webBrowser->setHtml(QString::fromUtf8(wikipediaArticle));
	} else {
		_webBrowser->setUrlLineEdit(url.toString());
		_webBrowser->setHtml(tr("Error: ") + ContentFetcher::errorString(error));
	}
}
