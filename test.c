#include "irclib.h"

int main(void)
{
	void *handle;
	struct timeval tm;
	fd_set readfs;

	handle = irclib_create_handle();
	irclib_setnick(handle, "IRClib");
	irclib_connect(handle, "irc.pfzt.net", 6667);

	while(!irclib_connected(handle));

	printf("Connected.\n");

	while(1) {
		tm.tv_sec = 2;
		tm.tv_usec = 150000;

		FD_ZERO(&readfs);
		irclib_select(fileno(stdin), &readfs, NULL, NULL, &tm);
	}
	return 0;
}
