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

#ifndef XMLFILEOUTPUT_H
#define XMLFILEOUTPUT_H

#include <Logger/LoggerOutput.h>
#include <Logger/LoggerExport.h>

class QString;
class QXmlStreamWriter;

/**
 * Writes the log output to a XML file.
 *
 * @author Tanguy Krotoff
 */
class LOGGER_API XmlFileOutput : public LoggerOutput {
	Q_OBJECT
public:

	/**
	 * XML root element: Log.
	 *
	 * <pre>
	 * <Log>
	 * 	<LogMessage>
	 * 	...
	 * 	</LogMessage>
	 * </Log>
	 * </pre>
	 */
	static const char * XML_ROOT_ELEMENT;

	XmlFileOutput(bool enabled, const QString & fileName);

	~XmlFileOutput();

	void write(const LogMessage & msg);

	QString name() const;

private:

	QXmlStreamWriter * _xmlWriter;
};

#endif	//XMLFILEOUTPUT_H
