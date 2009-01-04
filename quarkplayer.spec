#
# RPM spec file for QuarkPlayer
#

# Trick: define our own topdir so there is no need to be root to create
# the package
%define _topdir /home/tanguy/quarkplayer/trunk/rpmbuild

# Define our own RPM filename so we can add special informations to the name
%define _rpmfilename quarkplayer-0.2.5-rev674-linux-gcc43-release.rpm

Name:          quarkplayer
License:       GPL
Group:         Applications/Multimedia
Summary:       A Phonon media player
Version:       0.2.5
Release:       1
URL:           http://phonon-vlc-mplayer.googlecode.com/
BuildRoot:     %{_tmppath}/%{name}-%{version}-build
Source:        %{name}-%{version}.tar.bz2
#BuildRequires cannot work under non-RPM based distributions like Debian
#Uncomment it under RPM based distributions
#BuildRequires: libqt4-devel

%description
QuarkPlayer is a multimedia application that plays your music and videos.
QuarkPlayer can use different backends (DirectShow, MPlayer, Xine,
GStreamer, VLC...) thanks to Qt and the Phonon library.
It also relies on an advanced plugin system and is available under Windows,
Linux and soon Mac OS X.

%prep
# Extract the source and go into the source directory
# This is a bit tricky: I don't want to download QuarkPlayer' source code,
# to create a .tar.bz2 and then extract it: this is useless
# Let's directly use the current source code directory
# To perform this trick, rpmbuild/BUILD is a symlink to current source code
# directory
# -D : do not delete the directory before unpacking sources
# -T : do not perform default achive unpacking
# -n : set the name of build directory
%setup -D -T -n .

%build
# Build the source code
make

%install
# Install all files into the BuildRoot
make DESTDIR=%{buildroot} install

%clean
# Clean up the hard disc after build
rm -rf %{buildroot}

%files
# Copy all the files that come with 'make install'
%attr(-,root,root) /*

%changelog
* Sun Jan 4 2009 Tanguy Krotoff <tkrotoff@gmail.com>
  - First spec file
