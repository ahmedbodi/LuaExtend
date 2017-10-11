1.copy "sqlite3.exe" to svn root folder
2.then do follow comnand in command line tools
cd E:\svn_root
sqlite3.exe .svn/wc.db "select * from work_queue"
sqlite3.exe .svn/wc.db "delete from work_queue"
sqlite3.exe .svn/wc.db "delete from wc_lock"
svn cleanup