#include "irclib.h"

/* PROTO */
void
parse_message(void *handle, unsigned char *message)
{
	size_t len = strlen((char *)message);
	size_t dst;
	unsigned char *from;

	printf("INCOMING: %s\n", message);

	if(len > 4) {
		if(strncmp((char *)message, "PING ", 5) == 0) {
			pkt_t *pong;

			pong = pkt_init(len+3);
			pkt_addstr(pong, "PONG ");
			pkt_addraw(pong, message+5, len-5);
			pkt_addstr(pong, "\r\n");

			sendPkt(handle, pong);
			pkt_free(pong);
			printf("Sent PONG.\n");
			return;
		}
	}

	if(message[0] == ':') {
		dst = chrdist((char *)message, ' ');
		if(dst != 0) {
			from = malloc(dst);
			memcpy(from, message+1, dst-1);
			from[dst-1] = 0;
			printf("FROM: {%s}\n", from);
			free(from);
		} else {
			printf("ERROR: Server sent us something funky.\n");
			return;
		}
	}
}
