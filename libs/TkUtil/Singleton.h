/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2011  Tanguy Krotoff <tkrotoff@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SINGLETON_H
#define SINGLETON_H

/**
 * Interface for the singleton pattern.
 *
 * First version of this class was a template using Meyers implementation.
 * Unfortunately Singleton template cannot work across DLLs.
 * @see http://forum.hardware.fr/hfr/Programmation/C-2/singleton-dll-memoire-sujet_80324_1.htm
 * @see http://www.google.fr/search?hl=en&q=DLL+Singleton+C%2B%2B&btnG=Rechercher&aq=f&aqi=&aql=&oq=&gs_rfai=
 *
 * The only acceptable solution I see is to add <pre>static MyClass::instance()</pre> function inside each singleton.
 *
 * Meyers implementation is the way to go since most of the time it will be threadsafe cf this comment (Nov 2 '09):
 * "Looking at the next standard (section 6.7.4), it explains how static local initialization is thread safe.
 * So once that section of standard is widely implemented, Meyer's Singleton will be the preferred implementation.
 * [...] Most compilers already implement static initialization this way. The one notable exception is Microsoft Visual Studio."
 * @see http://stackoverflow.com/questions/1661529/is-meyers-implementation-of-singleton-pattern-thread-safe/1663229#1663229
 *
 * @see http://stackoverflow.com/questions/1008019/c-singleton-design-pattern
 * @see http://stackoverflow.com/questions/270947/can-any-one-provide-me-a-sample-of-singleton-in-c
 *
 * Example:
 * @code
 * //MyClass.h
 * #include <TkUtil/Singleton.h>
 *
 * class MyClass : public Singleton {
 * public:
 *
 * 	static MyClass & instance();
 *
 * 	void doSomething();
 *
 * private:
 * };
 *
 * //MyClass.cpp
 * MyClass & MyClass::instance() {
 * 	//Meyers implementation
 * 	//Guaranteed to be destroyed
 * 	static T instance;
 * 	return instance;
 * }
 * @endcode
 *
 * @author Tanguy Krotoff
 */
class Singleton {
public:

	/*
	This does not work across DLLs
	You should write this yourself

	static T & instance() {
		//Guaranteed to be destroyed
		static T instance;
		return instance;
	}
	*/

protected:

	Singleton() { }

private:

	Singleton(const Singleton &);
	Singleton & operator=(const Singleton &);
};

#endif	//SINGLETON_H
