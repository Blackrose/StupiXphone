#ifndef GSMLIB_H
#define GSMLIB_H

void init_gsm();
void at_send(char *command, char *result);
int recv_gsm(char *result);
void gsm_echo_mode(bool flag);
#endif
