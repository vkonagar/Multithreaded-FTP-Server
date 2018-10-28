# Multithreaded FTP server
A light-weight multithreaded FTP server, supports standardized FTP protocol features such as LIST, RETR, PORT etc. Currently, the implementation only supports FTP Active mode. The server helps perform large-scale NAT stress testing. 

### Setup and how to run
* 'make' to compile the code.
* 'ftp_server' runs the server

### File serve directory
* File serving directory 'FTP_FILES' must be created on previous (..) to the current directory.

### Misc details.
* A new thread is spawned for each client.
* ulimits of the server are raised.
* ulimit -s 512 will decrease the stack size from the default ( 8MB ) to 512KB.
* ulimit -n 100000 will increase number of open file descriptors for a process from 1024 to 100,000.
* Added ramdisk for the FTP_FILES directory which optimises file IO.
* Increase socket buffers for TCP.
* Thread monitoring support is added.

### Links 
* [1]: http://www.cyberciti.biz/faq/linux-increase-the-maximum-number-of-open-files/
* [2]: http://www.jamescoyle.net/how-to/943-create-a-ram-disk-in-linux
* [3]: http://www.cyberciti.biz/faq/linux-tcp-tuning/
* [4]: http://stackoverflow.com/questions/13988780/too-many-open-files-ulimit-already-changed
* [5]: http://askubuntu.com/questions/181215/too-many-open-files-how-to-find-the-culprit
