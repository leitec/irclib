/**  _        _ _ _
 ** (_)_ _ __| (_) |__
 ** | | '_/ _| | | '_ \
 ** |_|_| \__|_|_|_.__/
 **
 ** A simple library for creating IRC clients.
 **
 ** (C) 2005 by Claudio Leite
 **
 ** Please see the COPYING file for more details.
 **/

#include "irclib.h"

#define tok tokens->data

/* PROTO */
void
parse_message(void *handle, char *message)
{
	split_t		*tokens;
	int		numeric;

	tokens = i_split(message);

	if(strncmp(tok[0], "PING", 4) == 0) {
		pkt_t *pingpkt;

		pingpkt = pkt_init(5+strlen(tok[1]));
		pkt_addstr(pingpkt, "PONG ");
		pkt_addstr(pingpkt, tok[1]);
		send_cmdpkt(handle, pingpkt);
		pkt_free(pingpkt);
	} else {
		numeric = strtol(tok[1], (char **)NULL, 10);
		if(numeric == 0)
			parse_command(handle, message, tokens);
		else
			parse_numeric(handle, message, tokens, numeric);
	}
}

/* PROTO */
void
parse_command(void *handle, char *message, split_t *tokens)
{
	char           *nick = NULL, *host = NULL, *datastart;
	char		*from = tok[0] + 1;
	size_t          len;

	if (strchr((char *) from, '!') != NULL) {
		len = chrdist((char *) from, '!');
		nick = malloc(len + 1);
		memcpy(nick, from, len);
		nick[len] = 0;
		host = strchr((char *) from, '!');
		host++;
	}

	if (strncmp(tok[1], "JOIN", 4) == 0) {
		datastart = strchr((char *) message + 1, ':');
		if (((IRCLIB *) handle)->callbacks[IRCLIB_JOIN] != NULL)
			((IRCLIB *) handle)->callbacks[IRCLIB_JOIN] (handle, nick, host, datastart + 1);
	} else if (strncmp(tok[1], "PRIVMSG", 7) == 0) {
		char           *destptr;

		destptr = strchr((char *) message, ' ');
		if (destptr != NULL) {
			destptr = strchr(destptr + 1, ' ');


		}
		printf("%s\n", (char *) message);
	}

	if (nick != NULL)
		free(nick);
}

/* PROTO */
void
parse_numeric(void *handle, char *message, split_t *tokens, int numeric)
{
	unsigned char *datastart;

	switch (numeric) {
	case 001:
	case 002:
	case 003:
	case 004:
	case 005:
		break;
	case 372:
	case 375:
	case 376:
	case 422:
		datastart = (unsigned char *) strchr((char *) message + 1, ':');

		if (((IRCLIB *) handle)->callbacks[IRCLIB_MOTD] != NULL)
			((IRCLIB *) handle)->callbacks[IRCLIB_MOTD] (handle, datastart + 1);

		if (numeric == 422 || numeric == 376) {
			if (((IRCLIB *) handle)->callbacks[IRCLIB_READY] != NULL)
				((IRCLIB *) handle)->callbacks[IRCLIB_READY] (handle);
		}
		break;
	default:
		printf("!! %s\n", (char *) message);
	}
}
