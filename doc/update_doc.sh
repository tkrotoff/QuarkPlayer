# Clean previous documentation files
svn -R revert ../../doc/html/*
rm -rf ../../doc/html/*.*
rm -rf ../../doc/html/search/*.*

# Run Doxytag to create a list of tags for Qt
doxytag -t qt.tag /usr/share/doc/packages/libqt4/html

# Run Doxygen
cd ..
doxygen

# Run installdox in order to link Qt tags to the Qt online documentation
cd ../doc/html
./installdox -l qt.tag@http://qt.nokia.com/doc/4.6/

# Remove missing subversion files
svn status | grep '\!' | awk '{print $2;}' | xargs svn rm

# Add files not under version control
svn status | grep '\?' | awk '{print $2;}' | xargs svn add

# svn commit should be done manually
