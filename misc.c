#include "irclib.h"

/* PROTO */
int
getbyteorder(void)
{
	uint16_t        blah = 0x5533;
	uint8_t         tmp;

	memcpy(&tmp, &blah, 1);

	if (tmp == 0x55)
		return HOST_BIG_ENDIAN;
	else
		return HOST_LITTLE_ENDIAN;
}
