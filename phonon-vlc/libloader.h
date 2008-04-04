/*
 * VLC backend for the Phonon library
 * Copyright (C) 2007-2008  Tanguy Krotoff <tkrotoff@gmail.com>
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

#ifndef LIBLOADER_H
#define LIBLOADER_H

class QLibrary;

/**
 * Loads and resolves functions from a shared library at runtime.
 *
 * Inspired from Qt-4.4.0, files dbus_symbols.h and dbus_symbols.cpp
 *
 * @author Tanguy Krotoff
 */
class LibLoader {
public:

	LibLoader(const char * libName, const char * functionToTest);

	~LibLoader();

	/**
	 * Resolves a given function.
	 *
	 * @param name function to resolve
	 * @return pointer to the function or NULL
	 */
	void * resolve(const char * name);

private:

	bool load();

	QLibrary * _lib;

	const char * _libName;

	const char * _functionToTest;
};

#endif	//LIBLOADER_H
