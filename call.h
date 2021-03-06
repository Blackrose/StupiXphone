#ifndef CALL_H
#define CALL_H

typedef enum CALL_INIT_MACHINE{
    START_STATE = 0,
    HOLD_STATE = 1,
    DIAL_STATE,
    CNT_STATE,
    BSL_STATE,
    CALL_STATE,
    END_STATE,
};

typedef enum CPAS_STATE{
    ME_READY = 0,
    ME_NOT_USED,
    ME_UNKNOW,
    ME_RING,
    ME_CALL,
    ME_NOT_READY,
};


void gsm_call(char *number);
void gsm_dial(char *number);
void gsm_hangup();
void gsm_call_answer();
void gsm_call_check();

void update_gui_ready();
void update_gui_done();
int get_call_state();
void gsm_query_call(char *income_number);
void gsm_query_me_state();


#endif
