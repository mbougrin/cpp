
#include <main.hpp>
#include <iostream>

void					my_signal(void)
{
	signal( SIGINT, &ClassSocket::sighandler );
}

int						main(int ac, char **av)
{
	if (ac == 3) {
		ClassSocket		socket = ClassSocket(av[1], atoi(av[2]));
		my_signal();
		socket.mainloop();
	} else {
		printf("timeserver ${LISTENING IP} ${LISTENING PORT}\n");
	}
	return 0;
}