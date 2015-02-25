# Multithreaded FTP server
A tiny multithreaded FTP server, which is efficient, fast, scalable. It handles most of the major commands like LIST, RETR, PORT etc. It can handle only active mode. It is used to test a large number of client sessions through some middle box like NAT to the server and check for the performance.

### Setup and how to run
* run make to compile the code.
* run ftp_server( executable ) file.

### File serve directory
* it only serves files on the directory named FTP_FILES on previous (..) to the current directory where the exe is run.
* Directory can be changed in the code.

### Architecture details.
* A new thread is spawned for each client.
* ulimits of the server are raised.
* ulimit -s 512 will decrease the stack size from the default ( 8MB ) to 512KB.
* ulimit -n 100000 will increase the no of open file descriptors for a process from default(1024) to 1 Lakh.
* ramdisk is also added at the FTP_FILES directory to optimise the file IO.
* Also tune the socket buffers to allow lot of space for the TCP buffers.
* Monitoring thread monitors the number of threads currently live on the server, serving the clients.

### Links 
* [1]: http://www.cyberciti.biz/faq/linux-increase-the-maximum-number-of-open-files/
* [2]: http://www.jamescoyle.net/how-to/943-create-a-ram-disk-in-linux
* [3]: http://www.cyberciti.biz/faq/linux-tcp-tuning/
* [4]: http://stackoverflow.com/questions/13988780/too-many-open-files-ulimit-already-changed
* [5]: http://askubuntu.com/questions/181215/too-many-open-files-how-to-find-the-culprit
