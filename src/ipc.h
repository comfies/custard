#ifndef IPC_H
#define IPC_H

void process_command(char*);
unsigned short int parse_boolean(const char*);
unsigned int parse_unsigned_integer(const char*);
unsigned int parse_rgba_color(const char*);

#endif /* IPC_H */
