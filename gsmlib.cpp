#include "gsmlib.h"
#include "HardwareSerial.h"
#include "Arduino.h"
#include "circular_buffer.h"
#include "call.h"

extern Circular_Buffer *cb;
void init_gsm()
{
    Serial1.begin(9600);
}

void gsm_send(char *command, char *result)
{
    char tmp, *ptr = NULL;
    unsigned int flag = 0;
    int sz;
    int i, j, k;
    int ret;
    
    Serial1.write(command);
    delay(100);

#if 0
    Serial.println(&result[0]);
    Serial.println(&result[1]);
    Serial.println(&result[2]);
    
    i = 0;
    while(cgmm[i])
        Serial.println(cgmm[i++]);
#endif

}

int recv_gsm(char *result)
{
    int sz;
    int i, j, k;
    int ret;
    char cgmm[200];
    char tmp;
    char out[200];
    ElemType elem;
    
    memset(cgmm, 0, sizeof(cgmm));
    memset(&elem, 0, sizeof(ElemType));

    delay(100);
    sz = Serial1.available();
    if(sz <= 0)
        return 0;

    for(i = 0, j = 0; i < sz; i++){
#if 0
        tmp = char(Serial1.read());
        if(tmp == 0xd || tmp == 0xa)
            continue;
        else
            cgmm[j++] = tmp;
#else
        tmp = char(Serial1.read());
        cgmm[j++] = tmp;
#endif
    }

    if(j <= 0 || cgmm[0] == 0)
        return 0;

    memcpy(elem.data, cgmm, j);
    elem.len = j;

#if 0
    i = 0;
    while(i != j){
        Serial.print(elem.data[i++], HEX);
        Serial.print("\t");
    }
    Serial.println("\r\n");
    //debug_prt(cgmm, j);
    cgmm[j+1] = '\0';
    if(strcmp(cgmm, "RING") == 0){
        Serial.println(cgmm);
        gsm_call_check();
    }else{
        cb_write(cb, &elem);
    }
#endif
        cb_write(cb, &elem);

#if 0
    j = k = 0;
    for(i = 0; i < strlen(cgmm); i++){
        if(cgmm[i] == 0xd)
            continue;
        if(cgmm[i] == 0xa){
            *(result+j*100+k) = '\0';
            ++k;
            if(strcmp((result+j*100), "") != 0){
                j += 1;
                k = 0;
            }else{
                k = 0;
            }
        }else{
            *(result+j*100+k) = cgmm[i];
            ++k;
        }
    }
#endif
#if 0
    if(strcmp((result+j*100), "OK") == 0){
        ret = 0;
    }else
        ret = -1;
#endif
#if 0
    if(result[0]){
    Serial.println(&result[0]);
    Serial.println(&result[1]);
    Serial.println(&result[2]);
    }
    
#endif
#if 0
    if(k == 0 && j < 2)
        ret = 0;
    else
        ret = 1;
    return ret;
#endif

}

void debug_prt(char *buf, unsigned int len)
{
    int i;
    for(i = 0; i < len; i++){
        Serial.print(buf[i], HEX);
        Serial.print(" ");
    }
    Serial.print("\r\n");
}

/* --------------------------------*/
/**
 * @Synopsis : setting echo mode 
 *
 * @Param flag : true is enable echo mode, false is disable echo mode.
 */
/* --------------------------------*/
void gsm_echo_mode(bool flag)
{
    if(flag)
        gsm_send("ATE1\r\n", NULL);
    else
        gsm_send("ATE0\r\n", NULL);
}

void debug_gsm_cmd(char *command, unsigned int recv_mode)
{
    char tmp_buf[100];
    unsigned read_sz;

    if(!recv_mode){
        Serial1.write(command);
    }
    delay(100);
    read_sz = Serial1.available();
    if(read_sz > 0){
        memset(tmp_buf, 0, sizeof(tmp_buf));
        for(int i = 0; i < read_sz; i++){
            tmp_buf[i] = Serial1.read();
        }
        Serial.println(tmp_buf);
    }

}
