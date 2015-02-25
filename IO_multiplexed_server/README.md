# Multithreaded IO multiplexed FTP server
This is a variant of the FTP server, instead of 1 thread per client, it uses 1 thread per 20 clients and multiplexes the IO of those clients in one thread
### Links 
* [1]: http://www.cyberciti.biz/faq/linux-increase-the-maximum-number-of-open-files/
* [2]: http://www.jamescoyle.net/how-to/943-create-a-ram-disk-in-linux
* [3]: http://www.cyberciti.biz/faq/linux-tcp-tuning/
