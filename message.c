#include "irclib.h"

/* PROTO */
void
parse_message(void *handle, unsigned char *message)
{
	size_t len = strlen((char *)message);
	size_t dst, dst2;
	int numeric;
	unsigned char *from, *msgcode;

	/*printf("INCOMING: %s\n", message);*/

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

			dst2 = chrdist((char *)message+dst+1, ' ');
			msgcode = malloc(dst2+1);
			memcpy(msgcode, message+dst+1, dst2);
			msgcode[dst2] = 0;
			if(sscanf((char *)msgcode, "%d", &numeric) == 0) {
				parse_command(handle, message, from, msgcode);
			} else {
				parse_numeric(handle, message, numeric);
			}

			free(from);
			free(msgcode);
		} else {
			printf("ERROR: Server sent us something funky.\n");
			return;
		}
	}
}

/* PROTO */
void
parse_command(void *handle, unsigned char *message, unsigned char *from, unsigned char *msgcode)
{
	printf("%s\n", (char *)message);
}

/* PROTO */
void
parse_numeric(void *handle, unsigned char *message, int numeric) 
{
	unsigned char *name = NULL, *datastart;
	char *ptr1;
	size_t dst;

	/* 
	 * Ugly but works.
	 *
	 * Get the target/object name
	 * :server XXX <name> 
	 */

	ptr1 = strchr((char *)message, ' ');
	if(ptr1 != NULL) {
		ptr1 = strchr(ptr1+1, ' ');
		if(ptr1 != NULL) {
			dst = chrdist(ptr1+1, ' ');
			name = malloc(dst+1);
			memcpy(name, ptr1+1, dst);
		}
	}

	switch(numeric) {
	case 001:
	case 002:
	case 003:
	case 372:
	case 375:
		datastart = (unsigned char *)strchr((char *)message+1, ':');
		printf("** %s\n", datastart+1);
		break;
	case 004:
	case 005:
		break;
	default:
		printf("%s\n", (char *)message);
	}
}
