void irclib_join (void *handle, char *channel);
void irclib_part (void *handle, char *channel);
void irclib_privmsg (void *handle, char *target, char *message);
void irclib_quit (void *handle, char *quitmsg);
void           * irclib_create_handle (void);
void irclib_register_callback (void *handle, int event, void (*ptr) (void *,...));
int irclib_connected (void *handle);
IRCLIB_RET irclib_setnick (void *handle, char *nickname);
void irclib_setname (void *handle, char *name);
void irclib_setusername (void *handle, char *name);
IRCLIB_RET irclib_select (int nfds, fd_set * readfds, fd_set * writefds, fd_set * exceptfds, struct timeval * timeout);
void parse_message (void *handle, char *message);
void parse_command (void *handle, char *message, split_t *tokens);
void parse_numeric (void *handle, char *message, split_t *tokens, int numeric);
int getbyteorder (void);
char * tolower_str (const char *orig);
pkt_t          * pkt_init (size_t len);
void pkt_zero (pkt_t * pkt);
void pkt_free (pkt_t * pkt);
pkt_t          * pkt_initP (uint8_t * data, uint16_t len);
size_t pkt_empty (pkt_t * pkt);
size_t pkt_getoffset (pkt_t * pkt);
void pkt_skip (pkt_t * pkt, size_t skipnum);
void pkt_setoffset (pkt_t * pkt, size_t offset);
IRCLIB_RET pkt_add8 (pkt_t * pkt, uint8_t data);
IRCLIB_RET pkt_add16 (pkt_t * pkt, uint16_t val);
IRCLIB_RET pkt_add32 (pkt_t * pkt, uint32_t val);
IRCLIB_RET pkt_addraw (pkt_t * pkt, uint8_t * data, size_t len);
IRCLIB_RET pkt_addstr (pkt_t * pkt, char *data);
uint8_t pkt_get8 (pkt_t * pkt);
uint16_t pkt_get16 (pkt_t * pkt);
uint32_t pkt_get32 (pkt_t * pkt);
uint8_t        * pkt_getraw (pkt_t * pkt, size_t len);
uint8_t        * pkt_getstr (pkt_t * pkt, size_t len);
ssize_t socksend (void *handle, unsigned char *data, size_t len);
ssize_t sendPkt (void *handle, pkt_t * packet);
ssize_t send_cmdpkt (void *handle, pkt_t *packet);
size_t chrdist (char *str, char ch);
IRCLIB_RET irclib_connect (void *handle, char *server, uint16_t port);
void  i_free (split_t * s);
split_t        * i_split (char *str);
