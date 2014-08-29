#ifndef GSMLIB_H
#define GSMLIB_H

void init_gsm();
void gsm_send(char *command, char *result);
int recv_gsm(char *result);
void gsm_echo_mode(bool flag);
void debug_prt(char *buf, unsigned int len);
void debug_gsm_cmd(char *command, unsigned int recv_mode);
#endif
