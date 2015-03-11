QuarkPlayer, a Phonon media player

QuarkPlayer is a multimedia (audio and video) player based on Qt. QuarkPlayer can use different backends (DirectShow, MPlayer, Xine, GStreamer, VLC...) thanks to the Phonon library. It also relies on an advanced plugin system and is available under Windows, Linux and soon Mac OS X.

### Status

**March 2011: project on hold, working on other stuffs**

Still very buggy, mostly tested under Windows. Next step is to add unit tests and code coverage in order to get a rock solid code base.

### Features

- Yet another media player
- Default Phonon backend is [MPlayer](http://www.mplayerhq.hu/) thus QuarkPlayer gets all MPlayer features
- Any Phonon backend can be used: DirectShow, QuickTime, Xine, GStreamer...
- Video playback
- Audio playback
- Tested on large music libraries (320 GB)
- Fast file browser search, no need to create a database (Winamp 2 way to use)
- Plugin based: everything is a plugin (core is about 4000 LOC)
- Clean and simple code
- Lyrics from [LyricWiki.org](http://lyricwiki.org/)
- Cover arts from Amazon
- Current playing artist Wikipedia article
- Supported playlist formats: M3U, M3U8, XSPF, PLS, ASX, CUE, WPL

### TODO

- Fancy features
- SQL database
- Default MPlayer backend has some severe limitations (bad MP3 VBR length, no DVD navigation, no gapless...)
- Mac OS X
- [last.fm](http://www.last.fm/), [Jamendo](http://www.jamendo.com/) and [Magnatune](http://www.magnatune.com/) support
- Hardware support (iPods...)
- Network protocols (DAAP, DLNA...)
- Unit tests


# Requirements

- OS: GNU/Linux, Windows, Mac OS X
- Tested under: Windows (XP to 7), Ubuntu 9.10
- Language: C++
- License: GNU GPLv3+

## Dependencies

- [Qt](http://qt.nokia.com/) >= 4.6.2
- [CMake](http://cmake.org/) >= 2.8.1
- Compiler: GCC or MinGW or Visual C++ (from version 2005 to 2010)

### Dependencies provided with QuarkPlayer (no manual install needed)

- [Phonon](http://phonon.kde.org/)
- [TagLib](http://developer.kde.org/~wheeler/taglib.html)
- [MediaInfo](http://mediainfo.sourceforge.net/)
- [ZenLib](http://sourceforge.net/projects/zenlib/)
- [QtIOCompressor](http://qt.nokia.com/products/appdev/add-on-products/catalog/4/Utilities/qtiocompressor)
- [QtSingleApplication](http://qt.nokia.com/products/appdev/add-on-products/catalog/4/Utilities/qtsingleapplication/)
- [HMAC-SHA2](http://www.ouah.org/ogay/hmac/)

### Optional dependencies

- [NSIS](http://nsis.sourceforge.net/) >= 2.45

## GNU/Linux

If Qt is not present on your distribution (libqt4-devel packages), you will have to compile Qt yourself. Recent GNU/Linux distributions already provide Qt.

## Visual C++

Several versions are supported:

- Visual C++ 2005 (MSVC80) released in November 2005
- Visual C++ 2008 (MSVC90) released in November 2007
- Visual C++ 2010 (MSVC10) released in April 2010

Visual C++ (within Visual Studio) comes in different editions:

- Express - free for non commercial use
- Standard
- Professional
- Team System - comes with performance collection tools

Why Visual C++ over MinGW? Visual C++ is better for day to day development and produces better binaries. Visual C++ is the recommended compiler under Windows.

If you need to compile Qt, I use these options: `configure -no-qt3support -no-webkit -nomake examples -nomake demos`.

I recommend the use of [Qt Visual Studio Add-in](http://qt.nokia.com/downloads/visual-studio-add-in): this helps when debugging Qt classes (QString, QStringList...)

### Visual Studio Evaluation Copy

You can download an evaluation copy of Visual Studio:

- [Visual Studio 2010 Ultimate (90 days)](http://download.microsoft.com/download/2/4/7/24733615-AA11-42E9-8883-E28CDCA88ED5/X16-42552VS2010UltimTrial1.iso) (2.3 GB)
- [Visual Studio 2008 Team Suite (90 days)](http://www.microsoft.com/downloads/en/details.aspx?FamilyId=D95598D7-AA6E-4F24-82E3-81570C5384CB) (3.82 GB) [SP1](http://www.microsoft.com/downloads/details.aspx?FamilyId=FBEE1648-7106-44A7-9649-6D9F6D58056E&displaylang=en)
- [Visual Studio 2005 (180 days)](http://download.microsoft.com/download/6/f/5/6f5f7a01-50bb-422d-8742-c099c8896969/En_vs_2005_vsts_180_Trial.img) (3.33 GB)

After installing the evaluation copy, there is not much to do: the installer of the commercial/evaluation version sets up the environment and provides a 'Visual Studio Command Prompt'.

Under Visual C++ 2008, you will probably get `LINK : fatal error LNK1000: Internal error during IncrBuildImage`, see http://support.microsoft.com/kb/948127.

### Visual Studio Express Edition

/!\ The whole installation process of Visual Studio Express Edition is long and uneasy, I recommend to use an evaluation copy of Visual Studio or MinGW.

Here some informations about Visual C++ Express Edition (taken from [KDE TechBase](http://techbase.kde.org/index.php?title=Getting_Started/Build/KDE4/Windows/MS_Visual_Studio)):

- Install Visual C++ Express Edition: http://www.microsoft.com/express/download/
- Install Platform/Windows SDK: http://en.wikipedia.org/wiki/Microsoft_Windows_SDK#Obtaining_the_SDK
- Install DirectX SDK if you want to compile the DirectShow 9 backend: http://msdn.microsoft.com/en-us/directx/default.aspx

You need to setup you compiling environment, I use this small script: [setenv_*.bat](https://github.com/tkrotoff/QuarkPlayer/tree/master/build). Adapt this script to fit your own needs. You have to run it from a 'Command prompt' each time you want to compile something using Visual C++.

## MinGW

You can use MinGW instead of Visual C++.

- Install Qt MinGW SDK: http://qt.nokia.com/downloads/sdk-windows-cpp

This package already comes with MinGW, thus you don't need to install it independently.

## MacOS X

I didn't try QuarkPlayer under MacOS X (I don't own an Apple computer).
However, MPlayer runs under MacOS X and 99% of QuarkPlayer source code is multiplatform and depends only on Qt. Only need should be to adapt CMakeLists.txt files.

## MPlayer Phonon backend

Dependencies:

- Same as above +
- MPlayer >= 1.0rc2
  - Website: http://www.mplayerhq.hu/
  - Developers infos: http://www.mplayerhq.hu/DOCS/tech/slave.txt
  - Download link: http://www.mplayerhq.hu/design7/dload.html

MPlayer Phonon backend runs the MPlayer (mplayer.exe under Windows) process in slave mode.

- Under GNU/Linux, you should get a MPlayer package with your favorite GNU/Linux distribution.
- Under Windows, install [SMPlayer](http://smplayer.sourceforge.net/) which comes with a Windows build of MPlayer, phonon-mplayer looks for `mplayer/mplayer.exe`.

## VLC Phonon backend

/!\ QuarkPlayer does not come anymore with the VLC backend.
It is now developed on the KDE repository, see https://projects.kde.org/projects/kdesupport/phonon/phonon-vlc.

Dependencies:

- Same as above +
- VLC >= 1.0.1
  - Website: http://www.videolan.org/
  - Developers infos: http://wiki.videolan.org/Developers_Corner
  - Download link: http://nightlies.videolan.org/

VLC Phonon backend uses libvlc (http://wiki.videolan.org/Libvlc) in order to control VLC.


# How to build

Warning: be sure that your Qt installation matches the way you compile QuarkPlayer i.e if Qt is in debug mode, QuarkPlayer should be compiled in debug mode too. You cannot mix QuarkPlayer in release mode and Qt in debug mode (you will get an error message: "backend cannot be loaded" when running QuarkPlayer).

Everything generated by the `make`/`nmake` command is inside the directory `build`.

### Under GNU/Linux

```Shell
cd trunk
cd build
./build_make-debug.sh
make
make install

# Create a Debian package (you need to install fakeroot and debhelper)
make deb

# Create a RPM package (you need to install rpmbuild)
make rpm

# Create a generic .tar.bz2 file containing the binaries
make package
```

Example of build structure:
```
build/linux-gcc43-debug
build/linux-gcc43-debug/quarkplayer
build/quarkplayer-0.2.5-rev683-linux-gcc43-debug.deb
build/quarkplayer-0.2.5-rev683-linux-gcc43-debug.rpm
```

### Under Windows with Visual C++

```Shell
cd trunk
cd build
build_nmake-debug.bat
nmake
nmake install

# Create a NSIS setup (you need to install NSIS)
nmake nsis

# Create a generic .zip file containing the binaries
nmake package
```

Example of build structure:
```
build/win32-x86-msvc80-debug
build/win32-x86-msvc80-debug/quarkplayer.exe
build/quarkplayer-0.2.5-rev683-win32-x86-msvc80-debug.setup
build/quarkplayer-0.2.5-rev686-win32-x86-msvc80-debug.zip
```

For generating a Visual C++ project, there is `build/build_projectmsvc90-debug.bat`, however I recommend the process above.

### Under Windows with MinGW

```Shell
cd trunk
cd build
build_mingw-debug.bat
mingw32-make
mingw32-make install
```

Example of build structure:
```
build/win32-x86-mingw-debug
build/win32-x86-mingw-debug/quarkplayer.exe
```

## Run the Phonon backends

Qt looks for a specific location where Phonon .(dll|so) backends are installed. Qt will load the first backend found (alphabetically) from this location. Check Qt Plugins HowTo: http://doc.trolltech.com/main-snapshot/plugins-howto.html.

KDE allows you to configure the Phonon backend you want to use globally via `systemsettings`. Qt comes with `qtconfig` under GNU/Linux that let's you configure the preferred Phonon backend.

Qt looks for Phonon backends inside `$QTDIR/plugins/phonon_backend`and `$APPDIR/phonon_backend`.

## Use the MPlayer backend (phonon-mplayer) with your own Qt application

Copy phonon-mplayer.(dll|so) inside `MYAPP/phonon_backend/`.

It is not possible to specify the Phonon backend used at the application level (there no function like `Phonon::setPreferredBackend("the backend my app needs")`).

# Rationales, background and history of QuarkPlayer

Here my reasons for writing another media player...

Goals:
- Multimedia (video and audio playback)
- C++/Qt
- Fast
- Multiplatform
- OS integration
- Plugins system
- Phonon library

## History

Everything started in march 2008 when I've discovered the video player [SMPlayer](http://smplayer.sourceforge.net/). Until then I was using the well-known [VLC](http://www.videolan.org/) video player under Windows.

Compared to VLC (version 0.8.6 at that time), SMPlayer had a much better GUI based on Qt. (VLC 0.8.6 was based on [wxWidgets](http://wxwidgets.org/) before moving to Qt for 0.9 release and laters). I started to look at SMPlayer source code and found that there were no separation between the engine (which uses [MPlayer](http://www.mplayerhq.hu/)) and the GUI.
At the same time, [Phonon](http://phonon.kde.org/), a multimedia API for Qt4, started to get mature and I was seduced by this thin multimedia layer where you can implement different backends (DirectShow, Xine, GStreamer...)

I sent few mails to the SMPlayer author about patching his video player for using Phonon instead of his own engine. He was pretty interested at first and I sent to him several patches and even submit experimental VLC and MPlayer backends for Phonon inside SMPlayer SVN repository.

Unfortunately, _"SMPlayer is so oriented to MPlayer that probably it would be necessary to rewrite it again from zero to use Phonon instead, and currently Phonon is very basic and doesn't allow all things that MPlayer can do."_

So I moved the experimental VLC and MPlayer backends to a new repository and decided to create a simple media player in order to test my new backends...

## Rationales

QuarkPlayer is designed to be a multimedia player, meaning not only for videos but also suitable for audio playback, similar to Windows Media Player approach. I want to play my videos with the same application that I use for music playback.

### Back to the basics

Remember the time you were using Winamp 2 or X11Amp?
At the time you had 2 windows: the music playback window and the playlist window, a little drag and drop from your file manager and you were done. I have discovered that a lot of people (including me) are still using music players this way. Current QuarkPlayer focus is to get back to this simple approach and improve it.

That's why I've added a "File Browser" panel from where they can do drag & drop to the playlist. It also includes a fast search engine.

### Libraries dependencies

QuarkPlayer only depends on Qt, not on KDE libraries. I think that QuarkPlayer should be as fast as possible and base it on KDE libraries would slow down the soft specially under Windows. Adding an important dependency like KDE makes it difficult to compile the software under Windows. I don't see yet any argument to introduce a KDE dependency compare to Qt features.

Under GNU/Linux, QuarkPlayer can be compiled if needed with a minimal "visual" support for KDE. `TkUtil` tries to achieve this goal by using `KMainWindow` for example instead of `QMainWindow`. This process is totally transparent from the rest of the source code.

### Portability

QuarkPlayer compiles with Visual C++ 2005/2008, MinGW and GCC for GNU/Linux.
Unfortunately I don't have a Mac OS X computer, that's why there is no available version under this OS.

### C++

Popular players are fast: Winamp, Foobar2000, VLC... One could think of using Python/Qt instead of C++/Qt, but I think it would slow down QuarkPlayer startup time and increase memory consumption.

### Free software

Source code is under GPLv3+

### Skins support

I'm not a skins fan, I prefer good OS integration and this is what Qt brings. However Qt CSS support might be a [good start](http://doc.trolltech.com/main-snapshot/stylesheet.html).

### Phonon

Phonon is a very well written multimedia API: easy, portable, small footprint... I thank Matthias Kretz, the author, for developing such a good library!

### Name

QuarkPlayer name comes from [Quark](http://en.wikipedia.org/wiki/Quark) to keep it in physics area like [Phonon](http://en.wikipedia.org/wiki/Phonon).

### VLC and MPlayer backends

When it comes to multiplatform audio/video engines, you have the choice between VLC and MPlayer. Let's integrate both inside Phonon library!

## Architecture

- Pushing intelligence to the edge
- KISS Keep It Simple, Stupid
- Small is Beautiful!

### Everything are plugins

A media player can contain a lot of features. Instead of having a monolithic software that can be difficult to maintain over time, a better approach is to add features via a plugin system. QuarkPlayer core is under 4000 LOC.

### Create libraries whenever you can

QuarkPlayer comes with several small libraries that can be re-used:
- TkUtil, which is a utility library
- PlaylistParser for parsing m3u playlist and other formats
- ContentFetcher to download Internet contents (lyrics, Amazon cover, artist Wikipedia page)
- WebBrowser, a simple web browser based on QTextBrowser (so no QtWebKit dependency)


# Coding conventions

In English, a sentence starts with an upper case letter and ends with a dot. When writing !, there should be no space between the ! and the word ("Hello everybody!" is correct, "Hello everybody !" is considered not to be).
Conventions exist for other characters like `(` `)` `?` `,` `-`...
Over time the same kind of conventions appeared for programming languages.

The big picture is that you want your code to be easily readable by other developers. The idea is to code in a "standard" way that most people follow and like. Code conventions generally match the programming paradigm that is used.

A good start is to follow your framework or language conventions and make your code consistent with it.

If you use Qt, just mimic the [API](http://doc.trolltech.com/qq/qq13-apis.html), same for [Java](http://java.sun.com/docs/codeconv/), [C#](http://www.csharpfriends.com/Articles/getArticle.aspx?articleID=336) or [GTK+](http://developer.gnome.org/doc/guides/programming-guidelines/code-style.html).

These coding conventions are directly inspired from the Java coding style. Thus you should read Java coding style documents and books since this one is not exhaustive.

Here are some interesting documents that you should read:

- [Java Code Conventions by Sun Microsystems](http://java.sun.com/docs/codeconv/)
- [Writing Robust Java Code by AmbySoft](http://ambysoft.com/javaCodingStandards.pdf)

### From "Writing Robust Java Code"

_Coding standards are important because they lead to greater consistency within your code and the code of your teammates. Greater consistency leads to code that is easier to understand, which in turn means it is easier to develop and to maintain. This reduces the overall cost of the applications that you create._

_You have to remember that your code will exist for a long time, long after you have moved on to other projects. An important goal during development is to ensure that you can transition your work to another developer, or to another team of developers, so that they can continue to maintain and enhance your work without having to invest an unreasonable effort to understand your code. Code that is difficult to understand runs the risk of being scrapped and rewritten - I wouldn't be proud of the fact that my code needed to be rewritten, would you? If everyone is doing their own thing then it makes it very difficult to share code between developers, raising the cost of development and maintenance._

_Inexperienced developers, and cowboys who do not know any better, will often fight having to follow standards. They claim they can code faster if they do it their own way. Pure hogwash. They MIGHT be able to get code out the door faster, but I doubt it. Cowboy programmers get hung up during testing when several difficult-to-find bugs crop up, and when their code needs to be enhanced it often leads to a major rewrite by them because they're the only ones who understand their code. Is this the way that you want to operate? I certainly do not._

## Example

Here is an implementation of a String class that inherits from std::string (which is bad btw), file is named String.h

```C++
/*
 * Copyright
 */

#ifndef STRING_H
#define STRING_H

#include <string>

/**
 * String: std::string wrapper/helper.
 *
 * Inspired from the class QString from the Qt library.
 *
 * By the way, inheriting from std::string is not a good approach since
 * std::string has no virtual destructor.
 *
 * @see QString
 * @see std::string
 * @see java.lang.String
 * @author Tanguy Krotoff
 */
class String : public std::string {
public:

	String() : std::string() { }

	String(const char * str) : std::string(str) { }

	String(const std::string & str) : std::string(str) { }

	/**
	 * Converts a String to std::string.
	 *
	 * @return the converted String to std::string
	 */
	operator const std::string&() {
		return *this;
	}

	/**
	 * Converts this string to an int.
	 *
	 * @return the string converted to an int or 0 if failed to convert
	 */
	int toInteger() const;

	/**
	 * Converts all of the characters in this string to lower case.
	 *
	 * Example:
	 * <pre>
	 * String myString("WengO");
	 * str = myString.toLowerCase(); // str == "wengo"
	 * </pre>
	 *
	 * @return the string converted to lowercase
	 */
	std::string toLowerCase() const;

	/**
	 * Converts all of the characters in this string to upper case.
	 *
	 * Example:
	 * <pre>
	 * String myString("WengO");
	 * str = myString.toUpperCase(); // str == "WENGO"
	 * </pre>
	 *
	 * @return the string converted to uppercase
	 */
	std::string toUpperCase() const;

	/**
	 * Replaces every occurence of the string before with the String after.
	 *
	 * @param before occurence to find
	 * @param after the string that will replace the String before
	 * @param caseSensitive the search is case sensitive;
	 *        otherwise the search is case insensitive
	 */
	void replace(const std::string & before, const std::string & after, bool caseSensitive = true);

	/**
	 * Gets a string from a number.
	 *
	 * @param number number to convert into a string
	 * @return number converted to a string
	 */
	static std::string fromNumber(int number);

	/**
	 * URL-encodes a string.
	 *
	 * @param str the string to encode
	 * @return a string with all non-alphanumeric characters replaced by their
	 *         URL-encoded equivalent.
	 */
	static std::string encodeUrl(const std::string & str);

	/**
	 * URL-decodes a string.
	 *
	 * @param str the URL-encoded string to decode
	 * @return a string with all URL-encoded sequences replaced by their
	 *         ASCII equivalent
	 */
	static std::string decodeUrl(const std::string & str);
};

#endif // STRING_H
```

## A standard C++ header template

```C++
/*
 * Copyright
 */

#ifndef NAMESPACE_HEADER_H
#define NAMESPACE_HEADER_H

#include "..."

#include <...>

#include <iostream>
#include <string>
#include <cstring>
#include <cassert>
#include <cstdlib>

/**
 * Autobrief.
 *
 * More comments.
 *
 * @see ...
 * @see ...
 * @author ...
 * @author ...
 * @author ...
 * @version ...
 */
class Blah : public Blah1, public Blah2, Blah3 {
public:

	/**
	 * Autobrief.
	 *
	 * More comments.
	 *
	 * @param toto ...
	 * @exception Exception1 ...
	 * @exception Exception2 ...
	 */
	Blah(const Toto & toto) throw (Exception1, Exception2);

	~Blah();

	/**
	 * Autobrief.
	 *
	 * More comments.
	 *
	 * @return ...
	 */
	std::string getSomething() const {
		return _something;
	}

	/**
	 * Autobrief.
	 *
	 * More comments.
	 *
	 * @param something ...
	 */
	void setSomething(const Something & something) {
		_something = something;
	}

	/**
	 * Autobrief.
	 *
	 * More comments.
	 *
	 * @return true if ...; false otherwise
	 */
	bool isSomething() const {
		return _isSomething;
	}

	/**
	 * Autobrief.
	 *
	 * More comments.
	 *
	 * @return true if ...; false otherwise
	 * @deprecated
	 */
	bool hasSomething() const {
		return _hasSomething;
	}

protected:

	/**
	 * Autobrief.
	 *
	 * More comments.
	 */
	virtual void doSomething();

private:

	/**
	 * Autobrief.
	 */
	void computeSomething();

	/** Autobrief. */
	Something _something;

	/**
	 * Autobrief.
	 *
	 * More comments.
	 */
	bool _isSomething;

	/** Autobrief. */
	bool _hasSomething;
};

#endif // NAMESPACE_HEADER_H
```

## C++ filenames

C++ headers are of the form `MyClass.h`. C++ implementation files are of the form `MyClass.cpp`. Note the mixed case for filenames.

## Copyright

On each source file the copyright notice should be written.
The copyright starts with `/*` not `/**` which is reserved for documentation.

## Include guards

```C++
#ifndef NAMESPACE_HEADER_H
#define NAMESPACE_HEADER_H
...
#endif // NAMESPACE_HEADER_H
```

Avoid the use of `__MY_CLASS_H` it's useless, `MYCLASS_H` is enough.

## Includes

From the book Thinking in C++ (available freely on http://bruceeckel.com)

_If the order of header inclusion goes "from most specific to most general" then it's more likely that if your header doesn't parse by itself, you'll find out about it sooner and prevent annoyances down the road._

So the general organization should be:
```C++
#include "MyHeader.h"

#include <InternalHeaders.h>

#include <QtLibraryHeaders.h>
#include <WindowsHeaders.h>
#include <OtherHeaders.h>

#include <StandardC++Headers>

#include <StandardCHeaders>
```

Standard C++ headers are of the form:
```C++
#include <iostream>
#include <vector>
```
Note that there is no .h.

Standard C headers are of the form:
```C++
#include <cstring>
#include <cstdlib>
```
Note that there is no .h and all the headers start with a c.

For readability reasons separate include sections with a new line. Be careful when you use #include; Windows is not case sensitive, UNIX is.

Never do `using namespace` inside a header file.

## Classes

```C++
class MySuperHypraCoolClass {
public:
protected:
private:
};
```

## Methods

Public methods should be declared at the top of the class definition, followed by protected methods and finally private ones. Why public methods first and private methods last? Because nobody cares about private methods so they should be at the bottom, on the other hand everybody cares about public methods thus they should be at the top.

```C++
class MyClass {
public:

	void doSomethingGreat(...) {
		...
	}

protected:

private:
};
```

## Getters/Setters

Most object-oriented languages follow `std::string getBlaBla()` syntax. Qt developers on the other hand follow `QString blaBla()`. Just follow your framework conventions.

## Private members

With a leading underscore `_`. They have to be always private.

```C++
Class MyClass {
	...
private:

	MyClass * _myMemberVariable;
};
```

The advantage of this approach is that you immediately know that you are dealing with a field, avoiding the name hiding issue with parameters and locals. The main disadvantage is that this is not the standard set by Sun. Another option is to start private members by `m_`.

## Variables

```C++
MyClass * myVariable = new MyClass();
int i;
int j = 0;
MyClass tmp;
```

## Constants

```C++
static const char * MY_CONSTANT;
```

## Indentation

~~Tabs should be used. A tab is equal to eight spaces.~~ Mixing tabs and spaces should be avoided. But a tab of eight spaces is too long!
_The answer to that is that if you need more than 3 levels of indentation, you're screwed anyway, and should fix your program._ (CodingStyle from the Linux kernel source distribution)

## Code Documentation

Use Javadoc comments. We don't want to be dependent on a specific tool like Doxygen. Javadoc comments are recognized by all the documentation generators (Doxygen included).

Javadoc comments are inside the header file. Why? Because people like to read header files and to see the documentation at the same time. I don't like to open the header file (.h) plus to read a huge implementation file (.cpp) at the same time. Counterpart is that if you modify documentation from .h files, you might need to recompile most of you project.

If you don't know how to document source code check the examples and read [How to Write Doc Comments for the Javadoc Tool from Sun Microsystems](http://java.sun.com/j2se/javadoc/writingdoccomments/).

More documentation on [KDE TechBase](http://techbase.kde.org/Policies/Library_Documentation_Policy).
Keep in mind (CodingStyle from the Linux kernel source distribution):

_Comments are good, but there is also a danger of over-commenting. NEVER try to explain HOW your code works in a comment: it's much better to write the code so that the _working_ is obvious, and it's a waste of time to explain badly written code._

_Generally, you want your comments to tell WHAT your code does, not HOW. Also, try to avoid putting comments inside a function body: if the function is so complex that you need to separately comment parts of it, you should probably split it into smaller pieces. You can make small comments to note or warn about something particularly clever (or ugly), but try to avoid excess. Instead, put the comments at the head of the function, telling people what it does, and possibly WHY it does it._

## Tags (@tag)

|             |                                                              |                            |
 ------------ | ------------------------------------------------------------ | --------------------------
| @author     | Developer name                                               | name                       |
| @bug        | (Not from javadoc) A Bug description                         | description                |
| @deprecated | Marks a method as deprecated. Some IDEs will issue a compilation warning if the method is called. | text |
| @exception  | Documents an exception thrown by a method - same as @throws. | name description           |
| @param      | Defines a method parameter. Required for each parameter.     | name description           |
| @return     | Documents the return value. This tag should not be used for constructors or methods defined with a void return type. | description |
| @see        | Documents an association to another method or class.         | ClassName ClassName#method |
| @since      | Documents when a method was added to a class.                | number                     |
| @version    | Provides the version number of a class or method.            | number                     |

Originally taken from http://en.wikipedia.org/wiki/Javadoc

Specific tags from Doxygen for modules can be used:

@name description

@defgroup name description

@ingroup name

```C++
/**
 * @name description
 * @{
 */

/** @} */

/**
 * @defgroup group1 The First Group
 *
 * This is the first group
 */

/**
 * @ingroup group1
 */
```

## HTML tags

`<p>new paragraph`

`<pre>preformatted fragment very useful for including source code inside documentation</pre>`

`<img>image</img>`

## Other tags

TODO

FIXME

## Implementation source code

Example (String.cpp):

```C++
/*
 * Copyright
 */

#include "String.h"

#include <cctype>
#include <algorithm>
#include <sstream>
using namespace std;

int String::toInteger() const {
	int tmp;
	stringstream ss(c_str());
	ss >> tmp;
	return tmp;
}

String String::toUpperCase() const {
	String tmp(c_str());
	transform(tmp.begin(), tmp.end(), tmp.begin(), toupper);
	return tmp;
}

String String::toLowerCase() const {
	String tmp(c_str());
	transform(tmp.begin(), tmp.end(), tmp.begin(), tolower);
	return tmp;
}

void String::replace(const std::string & before, const std::string & after, bool caseSensitive) {
	// Copy this + before to tmp + before2
	string tmp(c_str());
	String before2(before);

	if (!caseSensitive) {
		// Converts tmp + before2 in lower case
		tmp = toLowerCase();
		before2 = before2.toLowerCase();
	}

	// Searches on tmp + before2 rather than this + before
	string::size_type pos = 0;
	while ((pos = tmp.find(before2, pos)) != string::npos) {
		// Replaces on this + tmp
		string::replace(pos, before2.length(), after);
		tmp.replace(pos, before2.length(), after);
		pos = pos + after.length();
	}
}

std::string String::fromNumber(int number) {
	string tmp;
	stringstream ss;
	ss << number;
	ss >> tmp;
	return tmp;
}

std::string String::decodeUrl(const std::string & str) {
	std::string out;
	std::string::const_iterator it;

	for (it = str.begin(); it != str.end(); it++) {
		if (*it == '%') {
			char entity[3] = {*(++it), *(++it), 0};
			int c = strtol(entity, NULL, 16);
			out += c;
		} else {
			out += *it;
		}
	}
	return out;
}

std::string String::encodeUrl(const std::string & str) {
	std::string out;
	std::string::const_iterator it;

	for (it = str.begin(); it != str.end(); it++) {
		if (!(isalpha(*it) || isdigit(*it))) {
			unsigned char highNibble = ((unsigned char) *it) >> 4;
			unsigned char lowNibble = ((unsigned char) *it) & 0x0F;
			out += '%';
			out += (highNibble < 0x0A ? '0' + highNibble : 'a' + highNibble - 0x0A);
			out += (lowNibble < 0x0A ? '0' + lowNibble : 'a' + lowNibble - 0x0A);
			continue;
		}
		out += *it;
	}
	return out;
}
```

## Braces

_The preferred way, as shown to us by the prophets Kernighan and Ritchie, is to put the opening brace last on the line, and put the closing brace first, thusly:_ (CodingStyle from the Linux kernel source distribution)

```C++
if (x is true) {
	we do y
}

if (x == y) {
	..
} else if (x > y) {
	...
} else {
	....
}
```

Always write braces otherwise it can introduce mistakes:

```C++
if (x is true)
	we do y
	we do z
else
	we do w
```

## Use new lines in your code

Use a single blank line to separate logical groups of code.

## Avoid endline comments

They are hard to format and to maintain. Comments at the beginning of a new line makes the code cleaner.

```C++
if (x is true) {
	we do y	// y blah blah
	we do z	// z blah blah
}

if (x is true) {
	// y blah blah
	we do y

	// z blah blah
	we do z
}
```

## if(...) or if (...)?

Let's people decide: http://www.google.com/codesearch

["if (" -> 6,080,000 answers](http://www.google.com/codesearch?hl=en&lr=&q=%22if+%28%22&btnG=Search)

["if(" -> 2,730,000 answers](http://www.google.com/codesearch?hl=en&lr=&q=%22if%28%22&btnG=Search)

Same for "if (...) {" and "if (...){"

[") {" -> 5,490,000 answers](http://www.google.com/codesearch?hl=en&lr=&q=%22%29+%7B%22&btnG=Search)

["){" -> 754,000 answers](http://www.google.com/codesearch?hl=en&lr=&q=%22%29%7B%22&btnG=Search)

["if (tmp" -> 150,000 answers](http://www.google.com/codesearch?hl=en&lr=&q=%22if+%28tmp%22&btnG=Search)

["if(tmp" -> 22,900 answers](http://www.google.com/codesearch?hl=en&lr=&q=%22if%28tmp%22&btnG=Search)

["if ( tmp" -> 8,000 answers](http://www.google.com/codesearch?hl=en&lr=&q=%22if+%28+tmp%22&btnG=Search)

["if( tmp" -> 5,000 answers](http://www.google.com/codesearch?hl=en&lr=&q=%22if%28+tmp%22&btnG=Search)

["char *" -> 5,380,000 answers](http://www.google.com/codesearch?hl=en&lr=&q=%22char+*%22&btnG=Search)

["char*" -> 1,710,000 answers](http://www.google.com/codesearch?hl=en&lr=&q=%22char*%22&btnG=Search)

"filename" lang:C++ case sensitive -> 551,000 answers

"fileName" lang:C++ case sensitive -> 203,000 answers

"filename" lang:Java case sensitive -> 184,000 answers

"fileName" lang:Java case sensitive -> 117,000 answers

General idea is that you want your code to be accessible to other developers, a good start is to code in a 'standard' way that most developers follow.

## Methods and functions

A perfect explanation is from the Linux kernel CodingStyle:

_Functions should be short and sweet, and do just one thing. They should (...) do one thing and do that well._

_The maximum length of a function is inversely proportional to the complexity and indentation level of that function. So, if you have a conceptually simple function that is just one long (but simple) case-statement, where you have to do lots of small things for a lot of different cases, it's OK to have a longer function._

_However, if you have a complex function, and you suspect that a less-than-gifted first-year high-school student might not even understand what the function is all about, you should adhere to the maximum limits all the more closely. Use helper functions with descriptive names (...)._

_Another measure of the function is the number of local variables. They shouldn't exceed 5-10, or you're doing something wrong. Re-think the function, and split it into smaller pieces. A human brain can generally easily keep track of about 7 different things, anything more and it gets confused. You know you're brilliant, but maybe you'd like to understand what you did 2 weeks from now._

In other words, "Keep it Simple, Stupid", "Keep It Short and Straight-forward" or if your prefer Einstein, _Things should be made as simple as possible, but no simpler._

## Libraries and applications organization

Directory hierarchy for libraries and applications are as follow:

`/include/*.h`
Include files with classes that needs to be public

`/include/$platform/*.h`
Classes for a specific platform, $platform is lowercase and can be win32, linux, mac...

`/src/*.cpp`
Private classes: implementation files (.cpp) + private include files (.h)

`/src/$platform/*.cpp *.rc`
Classes for a specific platform, $platform is lowercase and can be win32, linux, mac...

`/doc/`
Documentation directory (with subdirectories like html for Doxygen, together...)

`/tests/`
Test classes

`/bin/`
Binary files (.exe, .lib, .so, .a...)

`/debian/`
Standard Debian installer directory

`/rpm/*.spec *.spec.in`
RPM installer directory

`/nsis/`
NSIS installer directory

`/Doxyfile`
Standard configuration file for Doxygen

`/README`
Standard read me file

`/INSTALL`
Standard install file

`/TODO`
Standard to do file

`/COPYING`
Standard license file

`/AUTHORS`
Standard authors file

`/ChangeLog`
Standard change log file

`/*.pro`
Standard qmake file

`/SConscript`
Standard SCons file

`/CMakeLists.txt`
Standard CMake file

`/*.vcproj *.sln *.dsp`
Visual C++ project files

## Qt Specific issues

In order to avoid unnecessary include directories it is recommended to write:

```C++
#include <QtCore/QObject>
#include <QtGui/QMainWindow>
```

rather than:

```C++
#include <QObject>
#include <QMainWindow>
```

This way, only one include directory is needed during compilation: `-I$QTDIR/include` instead of `-I$QTDIR/include/QtCore`, `-I$QTDIR/include/QtGui`, `-I$QTDIR/include/QtNetwork`...

## Links

- [Ambysoft Coding Standards](http://www.ambysoft.com/javaCodingStandards.html)
- [Code Conventions for the Java Programming Language](http://java.sun.com/docs/codeconv/)
- [How to Write Doc Comments for the Javadoc Tool](http://java.sun.com/j2se/javadoc/writingdoccomments/)
- [The Joel Test: 12 Steps to Better Code](http://www.joelonsoftware.com/printerFriendly/articles/fog0000000043.html)
- [Simple Is Beautiful](http://www.gamesfromwithin.com/articles/0406/000024.html)
- [Designing Qt-Style C++ APIs](http://doc.trolltech.com/qq/qq13-apis.html)
- [Qt Api Design Principles](http://qt.gitorious.org/qt/pages/ApiDesignPrinciples)
- [The Little Manual of API Design](http://chaos.troll.no/~shausman/api-design/api-design.pdf)
- [Qt Coding Conventions](http://qt.gitorious.org/qt/pages/CodingConventions)
- [Qt Binary Compatibility Workarounds](http://qt.gitorious.org/qt/pages/BinaryCompatibilityWorkarounds)
