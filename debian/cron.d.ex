#
# Regular cron jobs for the quarkplayer package
#
0 4	* * *	root	[ -x /usr/bin/quarkplayer_maintenance ] && /usr/bin/quarkplayer_maintenance
