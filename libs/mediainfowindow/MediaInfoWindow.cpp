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

#include "MediaInfoWindow.h"

#include "ui_MediaInfoWindow.h"
#include "MediaInfoFetcher.h"

#include <contentfetcher/LyricsFetcher.h>

#include <webbrowser/WebBrowser.h>

#include <tkutil/TkIcon.h>
#include <tkutil/LanguageChangeEventFilter.h>

#include <QtGui/QtGui>

#include <QtCore/QDebug>
#include <QtCore/QtGlobal>
#include <QtCore/QUrl>

MediaInfoWindow::MediaInfoWindow(QWidget * parent)
	: QDialog(parent) {

	_mediaInfoFetcher = NULL;

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

	//open directory button
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
	_mediaInfoFetcher = mediaInfoFetcher;
	connect(_mediaInfoFetcher, SIGNAL(fetched()), SLOT(updateMediaInfo()));
}

void MediaInfoWindow::setLanguage(const QString & language) {
	_language = language;
}

void MediaInfoWindow::show() {
	QDialog::show();
	refresh();
}

void MediaInfoWindow::retranslate() {
	_webBrowser->setBackwardIcon(TkIcon("go-previous"));
	_webBrowser->setForwardIcon(TkIcon("go-next"));
	_webBrowser->setReloadIcon(TkIcon("view-refresh"));
	_webBrowser->setStopIcon(TkIcon("process-stop"));
	_webBrowser->setHomeIcon(TkIcon("go-home"));
	_webBrowser->setGoIcon(TkIcon("go-jump-locationbar"));
	_webBrowser->setOpenBrowserIcon(TkIcon("internet-web-browser"));

	_refreshButton->setIcon(TkIcon("view-refresh"));
	_refreshButton->setToolTip(tr("Refresh Informations"));

	_openDirectoryButton->setIcon(TkIcon("document-open-folder"));
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
		if (_mediaInfoFetcher->mediaInfo().fetched()) {
			updateMediaInfo();
		}
	}
}

void MediaInfoWindow::updateMediaInfo() {
	MediaInfo mediaInfo = _mediaInfoFetcher->mediaInfo();

	_openDirectoryButton->setEnabled(!mediaInfo.isUrl());

	QIcon icon;
	if (!mediaInfo.isUrl()) {
		static QFileIconProvider iconProvider;
		icon = iconProvider.icon(QFileInfo(mediaInfo.fileName()));
	} else {
		icon = TkIcon("document-open-remote");
	}
	_ui->fileTypeLabel->setPixmap(icon.pixmap(QSize(16, 16)));

	//General
	_ui->filenameLineEdit->setText(mediaInfo.fileName());

	//Metadata
	QString tmp = mediaInfo.metadataValue(MediaInfo::TrackNumber);
	_ui->trackLineEdit->setText(tmp);
	tmp = mediaInfo.metadataValue(MediaInfo::DiscNumber);
	_ui->discLineEdit->setText(tmp);
	tmp = mediaInfo.metadataValue(MediaInfo::Title);
	_ui->titleLineEdit->setText(tmp);
	tmp = mediaInfo.metadataValue(MediaInfo::Artist);
	_ui->artistLineEdit->setText(tmp);
	tmp = mediaInfo.metadataValue(MediaInfo::Album);
	_ui->albumLineEdit->setText(tmp);
	tmp = mediaInfo.metadataValue(MediaInfo::AlbumArtist);
	_ui->albumArtistLineEdit->setText(tmp);
	tmp = mediaInfo.metadataValue(MediaInfo::OriginalArtist);
	_ui->originalArtistLineEdit->setText(tmp);
	tmp = mediaInfo.metadataValue(MediaInfo::Year);
	_ui->yearLineEdit->setText(tmp);
	tmp = mediaInfo.metadataValue(MediaInfo::Genre);
	_ui->genreLineEdit->setText(tmp);
	tmp = mediaInfo.metadataValue(MediaInfo::Comment);
	_ui->commentLineEdit->setText(tmp);
	tmp = mediaInfo.metadataValue(MediaInfo::Composer);
	_ui->composerLineEdit->setText(tmp);
	tmp = mediaInfo.metadataValue(MediaInfo::Publisher);
	_ui->publisherLineEdit->setText(tmp);
	tmp = mediaInfo.metadataValue(MediaInfo::Copyright);
	_ui->copyrightLineEdit->setText(tmp);
	tmp = mediaInfo.metadataValue(MediaInfo::URL);
	_ui->urlLineEdit->setText(tmp);
	tmp = mediaInfo.metadataValue(MediaInfo::BPM);
	_ui->bpmLineEdit->setText(tmp);
	tmp = mediaInfo.metadataValue(MediaInfo::EncodedBy);
	_ui->encodedByLineEdit->setText(tmp);

	//MusicBrainz tags
	QString musicBrainzReleaseId = mediaInfo.metadataValue(MediaInfo::MusicBrainzReleaseId);
	QString musicBrainzArtistId = mediaInfo.metadataValue(MediaInfo::MusicBrainzArtistId);
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
	QString amazonASIN = mediaInfo.metadataValue(MediaInfo::AmazonASIN);
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
	QString fileSize = mediaInfo.fileSize();
	QString length = mediaInfo.length();
	QString bitrate = mediaInfo.bitrate();
	if (!fileSize.isEmpty() || !length.isEmpty() || !bitrate.isEmpty()) {
		QString fileTypeWikipedia(mediaInfo.fileType().wikipediaArticle);
		QString fileTypeName = mediaInfo.fileType().fullName;
		if (!fileTypeWikipedia.isEmpty()) {
			fileInfo += "<a href=\"http://" + _language + ".wikipedia.org/wiki/" + fileTypeWikipedia + "\">" + fileTypeName + "</a>";
		} else {
			fileInfo += fileTypeName;
		}
		if (!fileSize.isEmpty()) {
			fileInfo += ", " + fileSize + " " + tr("MB");
		}
		if (!length.isEmpty()) {
			fileInfo += ", " + length;
		}
		if (!bitrate.isEmpty() && mediaInfo.videoStreamCount() > 0) {
			//Don't show the overall bitrate if there is no video stream
			//in this case the overall bitrate has no interest
			fileInfo += ", " + bitrate + " " + tr("kbps");
		}
	}
	QString encodedApplication;
	if (!mediaInfo.encodedApplication().isEmpty()) {
		encodedApplication = br + tr("Encoded Application:") + " " + mediaInfo.encodedApplication();
	}

	//Audio
	QString audioStream;
	int audioStreamCount = mediaInfo.audioStreamCount();
	for (int audioStreamId = 0; audioStreamId < audioStreamCount; audioStreamId++) {
		QString audioBitrate = mediaInfo.audioStreamValue(audioStreamId, MediaInfo::AudioBitrate);
		QString audioBitrateMode = mediaInfo.audioStreamValue(audioStreamId, MediaInfo::AudioBitrateMode);
		QString audioSampleRate = mediaInfo.audioStreamValue(audioStreamId, MediaInfo::AudioSampleRate);
		QString audioBitsPerSample = mediaInfo.audioStreamValue(audioStreamId, MediaInfo::AudioBitsPerSample);
		QString audioChannelCount = mediaInfo.audioStreamValue(audioStreamId, MediaInfo::AudioChannelCount);
		QString audioCodecProfile = mediaInfo.audioStreamValue(audioStreamId, MediaInfo::AudioCodecProfile);
		QString audioCodec = mediaInfo.audioStreamValue(audioStreamId, MediaInfo::AudioCodec);
		QString audioLanguage = mediaInfo.audioStreamValue(audioStreamId, MediaInfo::AudioLanguage);
		QString audioEncodedLibrary = mediaInfo.audioStreamValue(audioStreamId, MediaInfo::AudioEncodedLibrary);

		if (!audioBitrate.isEmpty() || !audioBitrateMode.isEmpty() || !audioSampleRate.isEmpty() ||
			!audioBitsPerSample.isEmpty() || !audioChannelCount.isEmpty() ||
			!audioCodecProfile.isEmpty() || !audioCodec.isEmpty() ||
			!audioLanguage.isEmpty() || !audioEncodedLibrary.isEmpty()) {

			if (audioStreamCount > 1) {
				audioStream += br + tr("Audio Stream #%1:").arg(audioStreamId + 1);
			} else {
				audioStream += br + tr("Audio Stream:");
			}
			if (!audioLanguage.isEmpty()) {
				audioStream += " " + audioLanguage + ",";
			}
			if (!audioBitrate.isEmpty()) {
				audioStream += " " + audioBitrate + " " + tr("kbps");
			}
			if (!audioBitrateMode.isEmpty()) {
				audioStream += " " + audioBitrateMode;
			}
			if (!audioSampleRate.isEmpty()) {
				audioStream += ", " + audioSampleRate + " " + tr("kHz");
			}
			if (!audioBitsPerSample.isEmpty()) {
				audioStream += ", " + audioBitsPerSample + " " + tr("bits");
			}
			if (!audioChannelCount.isEmpty()) {
				audioStream += ", " + audioChannelCount + " " + tr("channels");
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
		QString videoBitrate = mediaInfo.videoStreamValue(videoStreamId, MediaInfo::VideoBitrate);
		QString videoWidth = mediaInfo.videoStreamValue(videoStreamId, MediaInfo::VideoWidth);
		QString videoHeight = mediaInfo.videoStreamValue(videoStreamId, MediaInfo::VideoHeight);
		QString videoFrameRate = mediaInfo.videoStreamValue(videoStreamId, MediaInfo::VideoFrameRate);
		QString videoFormat = mediaInfo.videoStreamValue(videoStreamId, MediaInfo::VideoFormat);
		QString videoCodec = mediaInfo.videoStreamValue(videoStreamId, MediaInfo::VideoCodec);
		QString videoEncodedLibrary = mediaInfo.videoStreamValue(videoStreamId, MediaInfo::VideoEncodedLibrary);

		if (!videoBitrate.isEmpty() || !videoWidth.isEmpty() || !videoHeight.isEmpty() ||
			!videoFrameRate.isEmpty() || !videoCodec.isEmpty() ||
			!videoEncodedLibrary.isEmpty()) {

			if (videoStreamCount > 1) {
				videoStream += br + tr("Video Stream #%1:").arg(videoStreamId + 1);
			} else {
				videoStream += br + tr("Video Stream:");
			}
			if (!videoBitrate.isEmpty()) {
				videoStream += " " + videoBitrate + " " + tr("kbps");
			}
			if (!videoWidth.isEmpty() && !videoHeight.isEmpty()) {
				videoStream += ", " + videoWidth + "*" + videoHeight;
			}
			if (!videoFrameRate.isEmpty()) {
				videoStream += ", " + videoFrameRate + " " + tr("FPS");
			}
			if (!videoFormat.isEmpty()) {
				videoStream += ", " + videoFormat;
			}
			if (!videoCodec.isEmpty()) {
				videoStream += " (" + videoCodec + ")";
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
		QString textLanguage = mediaInfo.textStreamValue(textStreamId, MediaInfo::TextLanguage);
		QString textFormat = mediaInfo.textStreamValue(textStreamId, MediaInfo::TextFormat);

		if (!textFormat.isEmpty() || !textLanguage.isEmpty()) {

			if (textStreamCount > 1) {
				textStream += br + tr("Text Stream #%1:").arg(textStreamId + 1);
			} else {
				textStream += br + tr("Text Stream:");
			}
			if (!textLanguage.isEmpty()) {
				textStream += " " + textLanguage;
			}
			if (!textFormat.isEmpty()) {
				textStream += ", " + textFormat;
			}
		}
	}

	//Sets the label
	_ui->formatInfoLabel->setText(
		fileInfo + encodedApplication +
		videoStream + audioStream + textStream
	);

	QString moreInfo;
	if (mediaInfo.fileType().name == FileType::WMA) {
		moreInfo += tr("Warning:") + br +
			tr("WMA is a proprietary, Windows specific audio codec that includes DRM and thus is not recommended") + br +
			tr("Use instead a well supported standard audio codec like MP3 or Ogg/Vorbis");
	}
	else if (mediaInfo.fileType().name == FileType::WMV) {
		moreInfo += tr("Warning:") + br +
			tr("WMV is a proprietary, Windows specific video codec that includes DRM and thus is not recommended") + br +
			tr("Use instead a well supported standard video codec like Xvid or Ogg/Theora");
	}
	_ui->moreInfoLabel->setText(moreInfo);

	//Refresh ThumbnailView
	QFileInfo filenameInfo(mediaInfo.fileName());
	if (filenameInfo.isDir()) {
		_ui->thumbnailView->setDir(filenameInfo.absoluteFilePath());
	} else {
		_ui->thumbnailView->setDir(filenameInfo.absolutePath());
	}
	_ui->thumbnailView->refresh();

	ContentFetcher::Track track;
	track.artist = mediaInfo.metadataValue(MediaInfo::Artist);
	track.title = mediaInfo.metadataValue(MediaInfo::Title);

	//Download the Wikipedia article
	tmp = track.artist;
	tmp.replace(" ", "_");
	tmp = QUrl::toPercentEncoding(tmp);
	if (!tmp.isEmpty()) {
		if (_language.isEmpty()) {
			//Back to english
			_language = "en";
		}
		_webBrowser->setSource("http://" + _language + ".wikipedia.org/wiki/" + tmp);
	} else {
		//Clears the webBrowser content
		_webBrowser->clear();
	}

	//Download the lyrics
	_ui->lyricsTextEdit->setHtml(tr("Looking for the lyrics..."));
	LyricsFetcher * lyricsFetcher = new LyricsFetcher(this);
	connect(lyricsFetcher, SIGNAL(found(const QByteArray &, bool)),
		SLOT(lyricsFound(const QByteArray &, bool)));
	connect(lyricsFetcher, SIGNAL(networkError(QNetworkReply::NetworkError)),
		SLOT(networkError(QNetworkReply::NetworkError)));

	//Clear lyrics tab
	lyricsFound(QByteArray(), true);

	lyricsFetcher->start(track);
}

void MediaInfoWindow::networkError(QNetworkReply::NetworkError errorCode) {
	_ui->lyricsTextEdit->setHtml("Network error: " + QString::number(errorCode));
}

void MediaInfoWindow::lyricsFound(const QByteArray & lyrics, bool accurate) {
	QString text = QString::fromUtf8(lyrics);
	if (!text.isEmpty()) {
		_ui->lyricsTextEdit->setHtml(text);
	} else {
		_ui->lyricsTextEdit->setHtml(tr("Error: no lyrics found"));
	}
}
