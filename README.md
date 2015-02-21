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

### Architecture details.
* A new thread is spawned for each client.
