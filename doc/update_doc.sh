# Clean previous documentation files
svn -R revert ../../doc/html/*
rm -rf ../../doc/html/*.*
rm -rf ../../doc/html/search/*.*

# Run Doxytag to create a list of tags for Qt
doxytag -t qt.tag /usr/share/doc/qt4-doc-html/html

# Run Doxygen
cd ..
doxygen

# Run installdox in order to link Qt tags to the Qt online documentation
cd ../doc/html
./installdox -l qt.tag@http://qt.nokia.com/doc/4.6/

# Remove missing subversion files
# FIXME If no file do be removed, this command line will print:
# "svn: Try 'svn help' for more info"
svn status | grep '\!' | awk '{print $2;}' | xargs svn rm

# Add files not under version control
# FIXME If no file do be removed, this command line will print:
# "svn: Try 'svn help' for more info"
svn status | grep '\?' | awk '{print $2;}' | xargs svn add

# Change mime-type
# FIXME -R parameter does not work despite being
# inside Subversion documentation
# Bug as been reported
svn -R propset svn:mime-type text/html *.html
svn -R propset svn:mime-type text/css *.css
svn -R propset svn:mime-type image/png *.png
svn -R propset svn:mime-type image/gif *.gif
svn -R propset svn:mime-type application/x-javascript *.js

# svn commit should be done manually
