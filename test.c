#include "irclib.h"

void irc_motd(void *handle, char *motd)
{
	printf("** %s\n", motd);
}

void irc_ready(void *handle)
{
	printf("** Ready.\n");
	irclib_join(handle, "#irclib");
}

void irc_join(void *handle, char *nick, char *host, char *channel)
{
	printf("** %s [%s] has joined %s.\n", nick, host, channel);
}

int main(void)
{
	void *handle;
	struct timeval tm;
	fd_set readfs;

	handle = irclib_create_handle();
	irclib_setnick(handle, "IRClib");
	irclib_connect(handle, "irc.pfzt.net", 6667);
	irclib_register_callback(handle, IRCLIB_MOTD, irc_motd);
	irclib_register_callback(handle, IRCLIB_READY, irc_ready);
	irclib_register_callback(handle, IRCLIB_JOIN, irc_join);

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
