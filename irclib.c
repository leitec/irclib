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

int             endianness;
IRCLIB_HANDLES *handles = 0;

/* PROTO */
void           *
irclib_create_handle(void)
{
	IRCLIB         *h;
	IRCLIB_HANDLES *tmp;
	int             xx;

	endianness = getbyteorder();

	h = malloc(sizeof(struct IRCLib));

	h->sock = -1;
	h->data = NULL;
	h->data_len = 0;
	h->connected = 0;
	h->nickname = NULL;
	h->hostname = NULL;
	h->isaway = 0;
	h->isidle = 0;
	h->buffered = 0;
	h->waiting_len = 0;

	for (xx = 0; xx < NUM_CALLBACKS; xx++)
		h->callbacks[xx] = NULL;

	if (handles == NULL) {
		handles = malloc(sizeof(IRCLIB_HANDLES));
		handles->handle = h;
		handles->next = NULL;
	} else {
		for (tmp = handles; tmp->next != NULL; tmp = tmp->next);

		tmp->next = malloc(sizeof(IRCLIB_HANDLES));
		tmp->next->next = NULL;
		tmp->next->handle = h;
	}

	irclib_setnick((void *) h, "NoName");
	h->realname = strdup("IRClib User");
	h->username = strdup("irclib");

	return (void *) h;
}

/* PROTO */
void
irclib_register_callback(void *handle, int event, void (*ptr) (void *,...))
{
	((IRCLIB *) handle)->callbacks[event] = ptr;
}

/* PROTO */
int
irclib_connected(void *handle)
{
	return ((IRCLIB *) handle)->connected;
}

/* PROTO */
IRCLIB_RET
irclib_setnick(void *handle, char *nickname)
{
	pkt_t          *nickpkt;

	if (((IRCLIB *) handle)->nickname != NULL)
		free(((IRCLIB *) handle)->nickname);

	((IRCLIB *) handle)->nickname = strdup(nickname);

	if (((IRCLIB *) handle)->connected == 1) {
		nickpkt = pkt_init(7 + strlen(nickname));
		pkt_addstr(nickpkt, "NICK ");
		pkt_addstr(nickpkt, nickname);
		pkt_addstr(nickpkt, "\r\n");

		sendPkt(handle, nickpkt);
		pkt_free(nickpkt);
	}
	return IRCLIB_RET_OK;
}

/* PROTO */
void
irclib_setname(void *handle, char *name)
{
	if (((IRCLIB *) handle)->realname != NULL)
		free(((IRCLIB *) handle)->realname);

	((IRCLIB *) handle)->realname = strdup(name);
}

/* PROTO */
void
irclib_sethostname(void *handle, char *name)
{
	if (((IRCLIB *) handle)->hostname != NULL)
		free(((IRCLIB *) handle)->hostname);

	((IRCLIB *) handle)->hostname = strdup(name);
}

/* PROTO */
void
irclib_setusername(void *handle, char *name)
{
	if (((IRCLIB *) handle)->username != NULL)
		free(((IRCLIB *) handle)->username);

	((IRCLIB *) handle)->username = strdup(name);
}

/* PROTO */
IRCLIB_RET
irclib_select(int nfds, fd_set * readfds, fd_set * writefds, fd_set * exceptfds, struct timeval * timeout)
{
	IRCLIB_HANDLES *tmp;
	IRCLIB_RET      ret = IRCLIB_RET_OK;

	ssize_t          bytesread, bytesparsed;
	ssize_t          nextcr, nextnl;
	int             nocr;
	int             maxfd = nfds;
	unsigned char   recvbuf[4097];
	unsigned char  *bufptr, *messagestr;

	for (tmp = handles; tmp != NULL; tmp = tmp->next) {
		if ((int) tmp->handle->sock > maxfd)
			maxfd = (int) tmp->handle->sock;
		if (tmp->handle->sock != -1)
			FD_SET(tmp->handle->sock, readfds);
	}

	if (select(maxfd + 1, readfds, writefds, exceptfds, timeout) == -1)
		return IRCLIB_RET_ERROR;

	for (tmp = handles; tmp; tmp = tmp->next) {
		if (tmp->handle->sock == -1)
			continue;

		if (FD_ISSET(tmp->handle->sock, readfds)) {
			bytesread = recv(tmp->handle->sock, recvbuf, 4096, 0);
			if (bytesread <= 0) {
				shutdown(tmp->handle->sock, 0x02);
				tmp->handle->sock = -1;
				tmp->handle->connected = 0;

				if (tmp->handle->callbacks[IRCLIB_ERROR])
					tmp->handle->callbacks[IRCLIB_ERROR] (tmp->handle, IRCLIB_ERROR_DISCONNECTED);

				return IRCLIB_RET_ERROR;
			}
			recvbuf[bytesread] = 0;
			bytesparsed = 0;

			if (tmp->handle->waiting_len > 0) {
				tmp->handle->data_len = tmp->handle->waiting_len + bytesread;
				tmp->handle->data = malloc(tmp->handle->data_len + 1);
				memcpy(tmp->handle->data, tmp->handle->buffered, tmp->handle->waiting_len);
				memcpy(tmp->handle->data + tmp->handle->waiting_len, recvbuf, bytesread);
				tmp->handle->data[tmp->handle->data_len] = 0;
				free(tmp->handle->buffered);
				tmp->handle->buffered = 0;
				tmp->handle->waiting_len = 0;

			} else {
				tmp->handle->data_len = bytesread;
				tmp->handle->data = malloc(bytesread + 1);
				memcpy(tmp->handle->data, recvbuf, bytesread);
				tmp->handle->data[tmp->handle->data_len] = 0;
			}

			bufptr = tmp->handle->data;

			while (bytesparsed < tmp->handle->data_len) {
				nextcr = chrdist((char *) bufptr, '\r');
				nextnl = chrdist((char *) bufptr, '\n');

				/*
				 * Fix by graue@oceanbase.org: Some servers
				 * omit \r after certain messages (against
				 * RFC specs, but oh well)
				 */

				if (nextnl < nextcr) {
					nextcr = nextnl;
					nocr = 1;
				} else {
					nocr = 0;
				}

				if (nextcr == 0) {
					tmp->handle->buffered = malloc(tmp->handle->data_len - bytesparsed + 1);
					tmp->handle->waiting_len = tmp->handle->data_len - bytesparsed;
					memcpy(tmp->handle->buffered, bufptr, tmp->handle->data_len - bytesparsed);
					tmp->handle->buffered[tmp->handle->waiting_len] = 0;
					break;
				}
				messagestr = malloc(nextcr + 1);
				memcpy(messagestr, bufptr, nextcr);
				messagestr[nextcr] = 0;

				bytesparsed += nextcr + 1 + !nocr;
				bufptr += nextcr + 1 + !nocr;

				parse_message(tmp->handle, (char *) messagestr);
				free(messagestr);
			}

			free(tmp->handle->data);
			tmp->handle->data_len = 0;
		}
	}

	return ret;
}
