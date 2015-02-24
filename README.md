# Multithreaded FTP server
A multithreaded FTP server, which is efficient, fast, scalable. It handles most of the major commands like LIST, RETR, etc.
### Features
* This is still in developing mode
* This will initiate one thread for each client. 
* This will handle only some commands like GET, CWD.
* This is mainly aimed for scaling test purposes.
* This will be accompanies by the multithreaded FTP client, to check the scaling test

### Setup and how to run
* run make to compile the code.
* run ftp_server( executable ) file.

### File serve directory
* it only serves files on the directory named FTP_FILES on previous (..) to the current directory where the exe is run.

### Architecture details.
* A new thread is spawned for each client.
* ulimits of the server are raised.
* ulimit -s 512 will decrease the stack size from the default ( 8MB ) to 512KB.
* ulimit -n 100000 will increase the no of open file descriptors for a process from default(1024) to 1 Lakh.
* ramdisk is also added at the FTP_FILES directory to optimise the file IO.
* Also tune the socket buffers to allow lot of space for the TCP buffers.

### Links 
* [1]: http://www.cyberciti.biz/faq/linux-increase-the-maximum-number-of-open-files/
* [2]: http://www.jamescoyle.net/how-to/943-create-a-ram-disk-in-linux
* [3]: http://www.cyberciti.biz/faq/linux-tcp-tuning/
