xcb_connection_t *xcb_connection;
xcb_screen_t *screen;

void commit(void);
unsigned short int initialize_xcb_connection(void);
unsigned short int finalize_xcb_connection(void);
