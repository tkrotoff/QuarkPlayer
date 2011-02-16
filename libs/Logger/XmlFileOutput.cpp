/*
 * Copyright (c) 2011 Tanguy Krotoff.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "XmlFileOutput.h"

#include "LogMessage.h"

#include "LoggerLogger.h"

#include <QtCore/QFile>
#include <QtCore/QXmlStreamWriter>

const char * XmlFileOutput::XML_ROOT_ELEMENT = "Log";

XmlFileOutput::XmlFileOutput(bool enabled, const QString & fileName)
	: LoggerOutput(enabled) {

	_xmlWriter = NULL;

	//Does not fail to open the file even if already open!
	//QtLockedFile can be a solution here
	QFile * file = new QFile(fileName);
	if (!file->open(QIODevice::WriteOnly | QIODevice::Text)) {
		LoggerCritical() << "Couldn't open file:" << fileName << "error:" << file->errorString();
	} else {
		_xmlWriter = new QXmlStreamWriter(file);
		_xmlWriter->setAutoFormatting(true);
		_xmlWriter->writeStartDocument();
		_xmlWriter->writeStartElement("Log");
	}
}

XmlFileOutput::~XmlFileOutput() {
	if (_xmlWriter) {
		_xmlWriter->writeEndElement();
		_xmlWriter->writeEndDocument();

		//Is it correct to do that?
		_xmlWriter->device()->close();
		delete _xmlWriter->device();
		delete _xmlWriter;
	}
}

void XmlFileOutput::write(const LogMessage & msg) {
	if (_xmlWriter) {
		msg.write(*_xmlWriter);
	}
}

QString XmlFileOutput::name() const {
	return tr("XML File Output");
}
