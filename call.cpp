#include "call.h"
#include "Arduino.h"
#include "circular_buffer.h"
#include "gsmlib.h"

extern Circular_Buffer *cb;
unsigned int call_init_state = END_STATE;
void gsm_incoming();


void gsm_dial(char *number)
{
    char buf[30];

    sprintf(buf, "ATD%s;\r\n", number);
    at_send(buf, NULL);

}

void gsm_hangup()
{
    ElemType elem;
    char buf[30];
    char tmp[30];
    
    call_init_state = HOLD_STATE;
    
    strcpy(buf, "ATH\r\n");
    at_send(buf, NULL);

    //recv_gsm(NULL);
    if(cb_isempty(cb))
        return;

        cb_read(cb, &elem);
        //Serial.println(elem.data);
        
        memset(tmp, 0, sizeof(tmp));
        memcpy(tmp, elem.data, elem.len);
        tmp[elem.len+1] = '\n';

        if(strcmp(tmp, "OK") == 0){
            call_init_state = END_STATE;
            return;
        }

}

unsigned int connect_cnt;
bool connect_flag = false;
unsigned int ok_flag = 0;
void gsm_call(char *number)
{
    char buf[30];
    char ok_res[30];
    int ret;
    int cnt = 3;
    ElemType elem;
    char tmp[30];

    {
        if(connect_flag)
            connect_cnt += 1;

        if(ok_flag == 1 && connect_cnt > 500){
            //Serial.println("busy line");
            call_init_state = BSL_STATE;
            return;
        }
    //recv_gsm(NULL);
    if(cb_isempty(cb) == 0){
        cb_read(cb, &elem);
        
        memset(tmp, 0, sizeof(tmp));

        // convert the hex data to string
        memcpy(tmp, elem.data, elem.len);
        tmp[elem.len+1] = '\0';
        
        if(strcmp(tmp, "OK") == 0){
            ok_flag += 1;
            connect_flag = true;
        }
        if(strcmp(tmp, "NO CARRIER") == 0){
            //Serial.println("Hold");
            call_init_state = HOLD_STATE;
            return;
        }
#if 0
    Serial.println(elem.data);
    Serial.println(elem.len);
#endif

    switch(ok_flag){
        case 1:
            //Serial.println("Dialing...");
            call_init_state = DIAL_STATE;
            break;
        case 2:
            //Serial.println("Connected");
            call_init_state = CNT_STATE;
            break;
        default:
            break;
    }
    }

    }

}

bool ring_flag = false;
void gsm_call_check()
{
    ElemType elem;
    char buf[30];
    char tmp[30];

    if(ring_flag == false){
            ring_flag = true;
            call_init_state = CALL_STATE;
            gsm_incoming();
    }
#if 0
    recv_gsm(NULL);

    if(cb_isempty(cb))
        return;

        cb_read(cb, &elem);
        //Serial.println(elem.data);

        memset(tmp, 0, sizeof(tmp));
        memcpy(tmp, elem.data, elem.len);
        tmp[elem.len + 1] = '\0';
        if(strcmp(tmp, "RING") == 0){
            ring_flag = true;
            call_init_state = CALL_STATE;
            gsm_incoming();
        }
#endif
}

void gsm_incoming()
{
    char buf[30];
    ElemType elem;
    int idx,dir,stat,mode,mpty,ret;
    char number[20];

    strcpy(buf, "AT+CLCC\r\n");
    at_send(buf, NULL);
            
    recv_gsm(NULL);
    cb_read(cb, &elem);
    Serial.println(elem.data);
#if 0 
    /*
     * AT+CLCC+CLCC: 1,1,4,0,0,"13720405960",161OK
     */
    memset(buf, 0, sizeof(buf));
    memcpy(buf, elem.data+14, 30);
    sscanf(buf, "%d,%d,%d,%d,%d,%s,161OK", &idx,&dir,&stat,&mode,&mpty,&number);
    Serial.println(number);
#endif
}

void gsm_call_answer()
{
    ElemType elem;
    char buf[30];
    char tmp[30];
    bool ring_flag = false;
    bool ans_flag = false;
    unsigned int ret = 0;
    
            strcpy(buf, "ATA\r\n");
            at_send(buf, NULL);
            ring_flag = false;
            ans_flag = true;
#if 1
        if(cb_isempty(cb) == 0 && ans_flag){
            memset(&elem, 0, sizeof(elem));
            cb_read(cb, &elem);
            memset(tmp, 0, sizeof(tmp));
            memcpy(tmp, elem.data, elem.len);
            tmp[elem.len + 1] = '\0';
            if(strcmp(tmp, "OK") == 0)
                call_init_state = CNT_STATE;
        }
#endif
#if 0 
        memset(tmp, 0, sizeof(tmp));
        memcpy(tmp, elem.data, elem.len);
        tmp[elem.len+1] = '\n';

        if(strcmp(tmp, "OK") == 0){
            call_init_state = END_STATE;
            return;
        }
#endif

}

int get_call_state()
{
    return call_init_state;
}
