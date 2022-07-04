#ifndef CLASS_SOCKET_HPP
# define CLASS_SOCKET_HPP

# include <sys/time.h>
# include <sys/types.h>
# include <unistd.h>
# include <sys/socket.h>
# include <stdlib.h>
# include <netinet/in.h>
# include <iostream>
# include <stdlib.h>
# include <sys/resource.h>
# include <netdb.h>
# include <arpa/inet.h>
# include <stdio.h>
# include <string>
# include <iostream>
# include <sstream>
# include <signal.h>
# include <string.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <thread>
#include <chrono>

# define BUF_SIZE		1024

# define MAX_USER		42

# define FD_FREE		0
# define FD_SERV		1
# define FD_CLIENT		2

struct							s_fds
{
	int							type;
	void						(*fct_read)(int);
	void						(*fct_write)(int);
	char						buf_read[BUF_SIZE + 1];
	char						buf_write[BUF_SIZE + 1];
};

class							ClassSocket
{
	public:
		ClassSocket(char *addr, int port);
		~ClassSocket();

		void					initfd(void);
		void					do_select(void);
		void					check_fd(void);
		void					mainloop(void);
		void					createsocket(void);
	
		static void				cleanclient(struct s_fds *fds);
		static void				clientread(int cs);
		static void				clientwrite(int cs);
		static void				acceptclient(int i);
		static void 			sighandler(int nb);
	private:
		int						_maxsd;
		int						_r;
		int						_port;
		char					*_addr;
		fd_set					_writefd;
		fd_set					_readfd;
		static int				_sd;
		static struct s_fds 	*_fds;
		static int				_client;
};

#endif
