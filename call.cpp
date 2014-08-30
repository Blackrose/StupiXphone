#include "call.h"
#include "Arduino.h"
#include "circular_buffer.h"
#include "gsmlib.h"

extern Circular_Buffer *cb;
unsigned int call_init_state = END_STATE;

extern int try_lock();
extern void unlock();

void gsm_dial(char *number)
{
    char buf[30];

    sprintf(buf, "ATD%s;\r\n", number);
    gsm_send(buf, NULL);

}

void gsm_hangup()
{
    ElemType elem;
    char buf[30];
    char tmp[30];
    
    call_init_state = HOLD_STATE;
    
    strcpy(buf, "ATH\r\n");
    gsm_send(buf, NULL);

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

int call_check_state = 0;
bool ring_flag = false;
void gsm_call_check()
{
    ElemType elem;
    int i, j;
    char tmp[30] = {0};
    char ring_str[5] = "RING";
#if 0
    if(ring_flag == false){
            ring_flag = true;
            call_init_state = CALL_STATE;
            gsm_incoming();
    }
#endif
#if 1
        switch(call_check_state){
            case 0:
                if(try_lock()){
                    call_check_state = 1;
                }
                break;
            case 1:
                if(!cb_isempty(cb)){
                    cb_read(cb, &elem);
#if 0
                    for(int i = 0; i < elem.len; i++)
                        Serial.println(elem.data[i], HEX);
#endif      
                    // \r\nRING\r\n
                    for(i = 0, j = 0; i < elem.len; i++){
                        if(elem.data[i] == 0xd || elem.data[i] == 0xa)
                            continue;
                        if(elem.data[i] == ring_str[j])
                            j += 1;
                    }
                    //Serial.println(j);
                    if(j >= 3){
                        ring_flag = true;
                        sprintf(tmp, "%s, %s\n", __FUNCTION__, elem.data);
                        Serial.print(tmp);
                    }
                }
                unlock();
                call_check_state = 0;
                break;
            default:
                break;
    }
#endif
}

int get_income_state = 0;
void gsm_query_call(char *income_number)
{
    char buf[30];
    ElemType elem;
    int idx,dir,stat,mode,mpty,ret;
    char number[20];

    if(!ring_flag)
        return;
    switch(get_income_state){
        case 0:
            if(try_lock()){
                gsm_send("AT+CLCC\r\n", NULL);
                get_income_state = 1;
            }
            break;
        case 1:
            if(!cb_isempty(cb)){
                cb_read(cb, &elem);
                if(strstr(elem.data, "+CLCC") == NULL)
                    goto FIN;
                // \r\n+CLCC: 1,1,4,0,0,"13720405960",161\r\nOK\r\n
                memset(buf, 0, sizeof(buf));
                memcpy(buf, elem.data+9, elem.len - 9 - 12);
                //Serial.println(buf);
#if 1
                sscanf(buf, "%d,%d,%d,%d,%d,%s\r\n", &idx,&dir,&stat,&mode,&mpty,&number);
                //Serial.println(number);
                strcpy(income_number, number);
                call_init_state = CALL_STATE;
#endif
            }
FIN:
            unlock();
            get_income_state = 0;
            break;
        default:
            break;
    }

}

int get_me_state = 0;
void gsm_query_me_state()
{
    char buf[30];
    ElemType elem;
    int me_state = -1;
    char number[20];

    switch(get_me_state){
        case 0:
            if(try_lock()){
                gsm_send("AT+CPAS\r\n", NULL);
                get_me_state = 1;
            }
            break;
        case 1:
            if(!cb_isempty(cb)){
                cb_read(cb, &elem);
                if(strstr(elem.data, "+CPAS") == NULL)
                    goto FIN;
                // \r\n+CPAS: 0\r\nOK\r\n
                memset(buf, 0, sizeof(buf));
                memcpy(buf, elem.data+2, elem.len - 2 - 6);
                sscanf(buf, "+CPAS: %d", &me_state);
                //Serial.println(buf);
            }
FIN:
            unlock();
            get_me_state = 0;
            break;
        default:
            break;
    }
#if 1
    switch(me_state){
        case ME_READY:
            if(ring_flag)
                call_init_state = END_STATE;
            break;
        case ME_RING:
            call_init_state = CALL_STATE;
            break;
        case ME_CALL:
            call_init_state = CNT_STATE;
            break;
        case -1:
            break;
        default:
            break;
    }
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
            gsm_send(buf, NULL);
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
