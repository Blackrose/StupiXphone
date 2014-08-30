#include "gsmlib.h"
#include "HardwareSerial.h"
#include "Arduino.h"
#include "circular_buffer.h"
#include "call.h"

//#define RECV_DEBUG

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
    int i;
    int ret;
    
    Serial1.write(command);
    delay(100);

#if 0
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

    delay(150);
    if((sz = Serial1.available()) > 0){

        for(i = 0, j = 0; i < sz; i++){
#if 0
        tmp = char(Serial1.read());
        if(tmp == 0xd || tmp == 0xa)
            continue;
        else
            cgmm[j++] = tmp;
#else
        tmp = char(Serial1.read()) & 0xff;
        cgmm[j++] = tmp;
#endif
        }
        if(cgmm[sz - 1] != 0xa || cgmm[sz - 2] != 0xd)
            goto ERROR;
#ifdef RECV_DEBUG
    Serial.println(sz);
    //Serial.println(cgmm);
    debug_prt(cgmm, sz);
#endif
        memcpy(elem.data, cgmm, j);
        elem.len = j;

        cb_write(cb, &elem);
    }

#if 0
    if(k == 0 && j < 2)
        ret = 0;
    else
        ret = 1;
    return ret;
#endif
ERROR:
    return sz;

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

    delay(100);
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
        //Serial.println(tmp_buf);
        debug_prt(tmp_buf, read_sz);
    }

}
