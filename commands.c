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

/* PROTO */
void
irclib_join(void *handle, char *channel)
{
	pkt_t          *pkt;

	pkt = pkt_init(5 + strlen(channel) + 2);

	pkt_addstr(pkt, "JOIN ");
	pkt_addstr(pkt, channel);
	pkt_addstr(pkt, "\r\n");

	sendPkt(handle, pkt);
	pkt_free(pkt);
}
