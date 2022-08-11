#include <ClassSocket.hpp>

using namespace std;
using namespace std::chrono;

int			ClassSocket::_sd;
struct s_fds		*ClassSocket::_fds;
int			ClassSocket::_client;

ClassSocket::ClassSocket(char *addr, int port) : _port(port), _addr(addr)
{
	_fds = NULL;
	_sd = 0;
	_client = 0;
	createsocket();
	return ;
}

ClassSocket::~ClassSocket(void)
{
	free(_fds);
	close(_sd);
	return ;
}

void				ClassSocket::sighandler(int nb)
{
	static bool sigint = false;

	if (nb == SIGINT) {
		if (!sigint) {
			for (int i = 0 ; i < MAX_USER ; ++i) {
				if (_fds[i].type == FD_CLIENT) {
					send(i, "bye\n", 4, MSG_DONTWAIT);
					close(i);
				}
			}
		}
		close(_sd);
		if (sigint) {
			exit(-1);
		}
		sigint = true;
	}
}

void				ClassSocket::initfd(void)
{
	int	i;

	i = 0;
	FD_ZERO(&_writefd);
	FD_ZERO(&_readfd);
	while (i < _maxsd)
	{
		if (_fds[i].type != FD_FREE) {
			FD_SET(i, &_readfd);
			FD_SET(i, &_writefd);
		}
		++i;
	}
}

void				ClassSocket::do_select(void)
{
	struct timeval	tv;

	tv.tv_sec = 1;
	_r = select(_sd + 1, &_readfd, &_writefd, NULL, &tv);
}

void				ClassSocket::check_fd(void)
{
	int		i = 0;

	while ((i < _maxsd) && (_r > 0)) {
		if (FD_ISSET(i, &_readfd)) {
			_fds[i].fct_read(i);
		}
		if (FD_ISSET(i, &_writefd)) {
			_fds[i].fct_write(i);
		}
		if (FD_ISSET(i, &_readfd)
		    || FD_ISSET(i, &_writefd)) {
			_r--;
		}
		i++;
	}
}

void				ClassSocket::mainloop(void)
{
	while (1) {
		initfd();
		do_select();
		check_fd();
	}
}

void				ClassSocket::cleanclient(struct s_fds *fds)
{
	fds->type = FD_FREE;
	fds->fct_read = NULL;
	fds->fct_write = NULL;
}

void				ClassSocket::clientread(int cs)
{
	int r;

	r = recv(cs, _fds[cs].buf_read, BUF_SIZE, 0);
	if (r <= 1) {
		ClassSocket::cleanclient(&_fds[cs]);
		close(cs);
		_client--;
	}
}

void				ClassSocket::clientwrite(int cs)
{
	write(cs, _fds[cs].buf_write, strlen(_fds[cs].buf_write));
}

void				thread_write(int cs)
{
	while (1) {
		struct timeval time_now{};
    		gettimeofday(&time_now, nullptr);
    		time_t msecs_time = (time_now.tv_sec * 1000) + (time_now.tv_usec / 1000);
		std::stringstream ss;
		ss << msecs_time;
		ss << "\n";
		std::string ts = ss.str();
		send(cs, ts.c_str(),
			 ts.size() + 1, 0);
		sleep(1);
	}
}

void				ClassSocket::acceptclient(int i)
{
	int					cs;
	struct sockaddr_in	csin;
	socklen_t			csin_len;

	(void)i;
	csin_len = sizeof(csin);
	if ((cs = accept(_sd, (struct sockaddr *)&csin, &csin_len)) == -1) {
		exit(-1);
	}
	ClassSocket::cleanclient(&_fds[cs]);
	if (_client >= MAX_USER) {
		close(cs);
		return ;
	}
	_fds[cs].type = FD_CLIENT;
	_fds[cs].fct_read = &ClassSocket::clientread;
	_fds[cs].fct_write = &ClassSocket::clientwrite;
	_client++;
	send(cs, "hey\n", 4, 0);
	thread t1(thread_write, cs);
     	t1.detach();
}

void				ClassSocket::createsocket(void)
{
	struct rlimit		rlp;
	struct sockaddr_in	sin;
	int                 optval;

	if (getrlimit(RLIMIT_NOFILE, &rlp) == -1) {
		exit(-1);
	}
	_maxsd = rlp.rlim_cur;
	_fds = (struct s_fds *)malloc(sizeof(struct s_fds) * _maxsd);
	if ((_sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		exit(-1);
	}
	sin.sin_port = htons(_port);
	sin.sin_addr.s_addr = inet_addr(_addr);
	sin.sin_family = AF_INET;
	optval = 1;
	if ((setsockopt(_sd,SOL_SOCKET,SO_REUSEADDR,&optval,sizeof(int))) == -1) {
		exit(-1);
	}
	if (bind(_sd, (struct sockaddr *)&sin, sizeof(struct sockaddr)) == -1) {
		exit(-1);
	}
	if (listen(_sd, _maxsd) == -1) {
		exit(-1);
	}
	_fds[_sd].type = FD_SERV;
	_fds[_sd].fct_read = &ClassSocket::acceptclient;
}
