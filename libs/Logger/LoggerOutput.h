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

#ifndef LOGGEROUTPUT_H
#define LOGGEROUTPUT_H

#include <QtCore/QObject>

class LogMessage;

class QString;

/**
 * Interface, plugin system for the Logger library.
 *
 * Inherits from QObject so translation can be used
 * + LogWindow needs to use QObject because of QSignalMapper
 * (slot/signal mechanism from Qt is not powerfull enough).
 *
 * @author Tanguy Krotoff
 */
class LoggerOutput : public QObject {
	Q_OBJECT
public:

	LoggerOutput(bool enabled);

	virtual ~LoggerOutput();

	/**
	 * Enable or disable the output.
	 *
	 * @param enabled true to enable the output; false to disable it
	 */
	void setEnabled(bool enabled);

	/**
	 * Tells if the output is enabled or not
	 *
	 * @return true if enabled; false if disabled
	 */
	bool isEnabled() const;

	/**
	 * Writes the LogMessage to the output.
	 */
	virtual void write(const LogMessage & msg) = 0;

	/**
	 * Output plugin name.
	 *
	 * The output plugin name will be shown inside the LogWindow.
	 */
	virtual QString name() const = 0;

private:

	/** if the output is enabled or not. */
	bool _enabled;
};

#endif	//LOGGEROUTPUT_H
