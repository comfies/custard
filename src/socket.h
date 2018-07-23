char socket_path[] = "/tmp/custard.sock";
int socket_file_descriptor;

unsigned short int initialize_socket(void);
void read_socket(void);
void *start_socket_read_loop(void *);
unsigned short int finalize_socket(void);

