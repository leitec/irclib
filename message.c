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
	size_t x;

	tokens = i_split(message);
	for(x = 0; x < tokens->num; x++)
		printf("[%s] ", tok[x]);

	printf("\n");

	if(strncmp(tok[0], "PING", 4) == 0) {
		pkt_t *pingpkt;

		pingpkt = pkt_init(5+strlen(tok[1]));
		pkt_addstr(pingpkt, "PONG ");
		pkt_addstr(pingpkt, tok[1]);
		send_cmdpkt(handle, pingpkt);
		pkt_free(pingpkt);
	}
}

/* PROTO */
void
parse_command(void *handle, unsigned char *message, unsigned char *from, unsigned char *msgcode)
{
	char           *nick = NULL, *host = NULL, *datastart;
	size_t          len;

	if (strchr((char *) from, '!') != NULL) {
		len = chrdist((char *) from, '!');
		nick = malloc(len + 1);
		memcpy(nick, from, len);
		nick[len] = 0;
		host = strchr((char *) from, '!');
		host++;
	}
	if (strcmp((char *) msgcode, "JOIN") == 0) {
		datastart = strchr((char *) message + 1, ':');
		if (((IRCLIB *) handle)->callbacks[IRCLIB_JOIN] != NULL)
			((IRCLIB *) handle)->callbacks[IRCLIB_JOIN] (handle, nick, host, datastart + 1);
	} else if (strcmp((char *) message, "PRIVMSG") == 0) {
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
parse_numeric(void *handle, unsigned char *message, int numeric)
{
	unsigned char  *name = NULL, *datastart;
	char           *ptr1;
	size_t          dst;

	/*
	 * Ugly but works.
	 *
	 * Get the target/object name :server XXX <name>
	 */

	ptr1 = strchr((char *) message, ' ');
	if (ptr1 != NULL) {
		ptr1 = strchr(ptr1 + 1, ' ');
		if (ptr1 != NULL) {
			dst = chrdist(ptr1 + 1, ' ');
			name = malloc(dst + 1);
			memcpy(name, ptr1 + 1, dst);
		}
	}
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
		printf("%s\n", (char *) message);
	}
}
