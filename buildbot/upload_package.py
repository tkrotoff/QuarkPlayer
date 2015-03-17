"""
  QuarkPlayer, a Phonon media player
  Copyright (C) 2008-2009  Tanguy Krotoff <tkrotoff@gmail.com>

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
"""

import sys, os, glob
import ftplib

def ftp_upload_files(host, login, password, host_path, files_to_upload):
	"""
	Uploads files onto a FTP server in binary mode.
	"""

	ftp = ftplib.FTP(host)
	ftp.login(login, password)

	for file_to_upload in files_to_upload:
		print 'upload {0} to ftp://{1}@{2}:{3}'.format(file_to_upload, login, host, host_path)
		file = open(file_to_upload, 'rb')
		destpath = os.path.join(host_path, os.path.basename(file_to_upload))
		ftp.storbinary('STOR ' + destpath, file)

		mode = '644'
		print 'chmod {0} {1}'.format(mode, file_to_upload)
		ftp.voidcmd('SITE CHMOD ' + mode + ' ' + destpath)

	ftp.quit()

if __name__ == "__main__":
	loginfile = 'login.txt'
	file = open(loginfile, 'r')
	login = file.readline().strip()
	password = file.readline().strip()
	file.close()
	os.remove(loginfile)

	files_to_upload = []
	for i, pattern in enumerate(sys.argv):
		if i > 0:

			# Fix a bug under Windows,
			# this script gets called with these arguments:
			# ['upload_package.py', "'*.exe'", "'*.deb'", "'*.rpm'"]
			# instead of ['upload_package.py', '*.exe', '*.deb', '*.rpm']
			pattern = pattern.replace('\'', '')
			pattern = pattern.replace('\"', '')

			files_to_upload.extend(glob.glob(pattern))

	ftp_upload_files('192.168.0.12', login, password, '/var/www/snapshots/', files_to_upload)

	for file_to_upload in files_to_upload:
		print 'rm {0}'.format(file_to_upload)
		os.remove(file_to_upload)
