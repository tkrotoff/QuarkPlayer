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

#ifndef SINGLETON_H
#define SINGLETON_H

/**
 * Template class to create singleton pattern.
 *
 * Becareful in multi-threaded environment: this class does not contain a mutex.
 *
 * Example:
 * <pre>
 * #include <tkutil/Singleton.h>
 *
 * class MyClass : public Singleton<MyClass> {
 * 	friend class Singleton<MyClass>;
 * public:
 *
 * 	void doSomething();
 *
 * private:
 *
 * 	MyClass();
 * 	~MyClass();
 * };
 * </pre>
 *
 * @author Tanguy Krotoff
 */
template<typename T>
class Singleton {
public:

	static T & instance() {
		if (!_instance) {
			_instance = new T;
		}
		return *_instance;
	}

	static void deleteInstance() {
		delete _instance;
		_instance = NULL;
	}

protected:

	Singleton() { }

	~Singleton() { }

private:

	Singleton(const Singleton &);
	Singleton & operator=(const Singleton &);

	static T * _instance;
};

template<typename T> T * Singleton<T>::_instance = NULL;

#endif	//SINGLETON_H
