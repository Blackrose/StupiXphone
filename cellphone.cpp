#include "Arduino.h"
#include "lcd.h"
#include "cellphone.h"
#include "HardwareSerial.h"
#include "gsmlib.h"
#include "circular_buffer.h"
#include "call.h"

#define ARDUINO 150
#include "Keypad.h"

#define PRINT(arg) (Serial.println(arg))

Circular_Buffer *cb;

Keypad keypad = Keypad(makeKeymap(hexkey), row_pins, col_pins, ROWS, COLS);

void home_proc(char keycode);
void sms_proc(char keycode);
void phonebook_proc(char keycode);
void callrecord_proc(char keycode);
void setting_proc(char keycode);
void dial_proc(char keycode);
void sysinfo_proc(char keycode);
void smsitem_proc(char keycode);

struct menuitem phonebook_menu;
struct menuitem callrecord_menu;

struct UILabel sms_label[] = {
    {"SMS", 30, 2,},
};

struct UILabel setting_label[] = {
    {"Setting", 20, 2,},
};

struct UILabel call_label[] = {
    {"", 0, 2,}
};
struct UILabel sysinfo_label[] = {
    {"ModuleInfo", 0, 0},
    {"SysInfo", 0, 1,},
};

struct UILabel smsitem_label[] = {
    {"WriteSMS", 0, 0},
    {"SendSMS", 0, 1,},
};

struct UILabel enter_btn = {
    "Enter", 0, 5,
};

struct UILabel quit_btn = {
    "Quit", 55, 5,
};

struct UILabel menu_btn = {
    "Menu", 0, 5,
};

struct UILabel goto_btn = {
    "Goto", 55, 5,
};

struct UILabel dial_btn = {
    "Oper", 0, 5,
};

struct UILabel clean_btn = {
    "Clean", 45, 5,
};

struct menuitem sendsms_menu = {
    .name = SYSINFO_SCN,
    .label = smsitem_label,
    .statusbar_enable = 0,
    .left = NULL,
    .right = NULL,
    .proc = &smsitem_proc,
    .left_btn = &enter_btn,
    .right_btn = &quit_btn,
    .label_cnt = 2,
};

struct menuitem sysinfo_menu = {
    .name = SYSINFO_SCN,
    .label = sysinfo_label,
    .statusbar_enable = 0,
    .left = NULL,
    .right = NULL,
    .proc = &sysinfo_proc,
    .left_btn = &enter_btn,
    .right_btn = &quit_btn,
    .label_cnt = 2,
};

struct menuitem setting_menu = {
    .name = SETTING_SCN,
    .label = setting_label,
    .statusbar_enable = 0,
    .left = &sysinfo_menu,
    .right = NULL,
    .proc = &setting_proc,
    .left_btn = &enter_btn,
    .right_btn = &quit_btn,
    .label_cnt = 1,
};

struct menuitem sms_menu = {
    .name = SMS_SCN,
    .label = sms_label,
    .statusbar_enable = 0,
    .left = &setting_menu,
    .right = NULL,
    .proc = &sms_proc,
    .left_btn = &enter_btn,
    .right_btn = &quit_btn,
    .label_cnt = 1,
};


struct menuitem dial_screen = {
    .name = DIAL_SCN,
    .label = call_label,
    .statusbar_enable = 1,
    .left = NULL,
    .right = NULL,
    .proc = &dial_proc,
    .left_btn = &dial_btn,
    .right_btn = &clean_btn,
    .label_cnt = 1,
};

struct UILabel home_label[] = {
    {MODULE_INFO.module_operator, 15, 2,},
};

struct menuitem home_screen = {
    .name = HOME_SCN,
    .label = home_label,
    .statusbar_enable = 1,
    .left = &dial_screen,
    .right = NULL,
    .proc = &home_proc,
    .left_btn = &menu_btn,
    .right_btn = &goto_btn,
    .label_cnt = 1,
};

struct menuitem *current_menu = NULL;
struct menuitem *root_screen = NULL;

void get_vendor_name();
void get_operator(char *);
void get_module_sn();
void get_sim_stat();
void get_operator();
void get_module_id();
void get_netregist_stat();
void gsm_sendsms();

void status_bar(){
    lcd_set_xy(0, 0);
    lcd_char(SIGNAL_WIDGET);

    lcd_set_xy(66, 0);
    lcd_char(BATERY_WIDGET);
}

void menu_screen()
{
    int i;

    //lcd_clear();
  
    if(current_menu->statusbar_enable)
        status_bar();
   
    if(current_menu->label){
        for(i = 0; i < current_menu->label_cnt; i++){
        lcd_set_xy(current_menu->label[i].pos_x, current_menu->label[i].pos_y);
        lcd_string(current_menu->label[i].text);
        }
    }


    if(current_menu->left_btn){
        lcd_set_xy(current_menu->left_btn->pos_x, current_menu->right_btn->pos_y);
        lcd_string(current_menu->left_btn->text);
    }
   
    if(current_menu->right_btn){
        lcd_set_xy(current_menu->right_btn->pos_x, current_menu->right_btn->pos_y);
        lcd_string(current_menu->right_btn->text);
    }
}

struct menuitem* search_node(struct menuitem *node, unsigned int name)
{
    struct menuitem *ptr;

    if(node == NULL)
        return NULL;

    if(node->name == name)
        return node;
    else{
        if(ptr = search_node(node->left, name))
            return ptr;
        
        if(ptr = search_node(node->right, name))
            return ptr;

        return NULL;
    }
}

void home_proc(char keycode)
{
    switch(keycode){
        case KEY_ENTER:
            current_menu = &sms_menu;
            update_gui_ready();
            break;
        case KEY_CALL:
            current_menu = &dial_screen;
            update_gui_ready();
            break;
        case KEY_QUIT:
            current_menu = &home_screen;
        default:
            break;
    }
            menu_screen();
        //lcd_set_xy(current_menu->label[0].pos_x, current_menu->label[0].pos_y);
        //lcd_string(current_menu->label[0].text);

#if 1
    if(get_call_state() != END_STATE){
        current_menu = &dial_screen;
        update_gui_ready();
    }
#endif
}

void sms_proc(char keycode)
{
    if(keycode == KEY_ENTER){
        current_menu = &sendsms_menu;
        update_gui_ready();
    }

    if(keycode == KEY_RIGHT){
        current_menu = &setting_menu;
        update_gui_ready();
        //menu_screen();
    }

    if(keycode == KEY_LEFT){
        current_menu = &sms_menu;
        update_gui_ready();

        //menu_screen();
    }

    if(keycode == KEY_QUIT){
        current_menu = &home_screen;
        update_gui_ready();
        //menu_screen();
    }
        menu_screen();

}

void smsitem_proc(char keycode)
{

    if(keycode == KEY_ENTER){
        gsm_sendsms();
        current_menu = &sms_menu;
        update_gui_ready();
    }

    if(keycode == KEY_QUIT){
        current_menu = &home_screen;
        update_gui_ready();
        //menu_screen();
    }

    menu_screen();
}

void setting_proc(char keycode)
{
    if(keycode == KEY_RIGHT){
    }

    if(keycode == KEY_LEFT){

    }

    if(keycode == KEY_ENTER){
        current_menu = &sysinfo_menu;
        update_gui_ready();
    }

    if(keycode == KEY_QUIT){
        current_menu = &home_screen;
        update_gui_ready();
    }
        menu_screen();

}

void sysinfo_proc(char keycode)
{
    if(keycode == KEY_RIGHT){
    }

    if(keycode == KEY_LEFT){

    }

    if(keycode == KEY_ENTER){
        current_menu = search_node(root_screen, SYSINFO_SCN);
    }

    if(keycode == KEY_QUIT){
        current_menu = &home_screen;
        update_gui_ready();
    }
        menu_screen();

}

char income_number[20] = {0};
unsigned int dial_index = 0;
char dial_number[20] = "\0";
bool dial_flag = false;
bool hangup_flag = false;
void dial_proc(char keycode)
{
        
    if(keycode == KEY_QUIT){
        if(dial_flag)
            gsm_hangup();
        lcd_clear();
        memset(dial_number, '\0', sizeof(dial_number));
        dial_index = 0;
        current_menu = &home_screen;
        update_gui_ready();
        //menu_screen();
    }

    if(keycode == KEY_CALL){
        switch(get_call_state()){
            case END_STATE:
                gsm_dial(dial_number);
                dial_flag = true;
                break;
            case CALL_STATE:
                gsm_call_answer();
                break;
            default:
                break;
        }
    }

    if(keycode == KEY_HOLD)
        hangup_flag = true;
       
    if(dial_flag){
        gsm_call(dial_number);
    }

    if(dial_flag && hangup_flag)
        gsm_hangup();

    switch(keycode){
        case KEY_1:
        case KEY_2:
        case KEY_3:
        case KEY_4:
        case KEY_5:
        case KEY_6:
        case KEY_7:
        case KEY_8:
        case KEY_9:
        case KEY_0:
            dial_number[dial_index++] = keycode;
            break;
        default:
            break;
    }

    lcd_set_xy(0, 1);
    lcd_string(dial_number);
  
    // update the call state to screen
    switch(get_call_state()){
        case DIAL_STATE:
            call_label[0].text = "Dialing";
            break;
        case CNT_STATE:
            call_label[0].text = "Connected";
            break;
        case BSL_STATE:
            call_label[0].text = "Busy Line";
            break;
        case HOLD_STATE:
            call_label[0].text = "Hangup";
            break;
        case END_STATE:
            call_label[0].text = "";
            break;
        case CALL_STATE:
            {
            char buf[30] = {0};
            strcpy(buf, "IncomingCall");
            strcat(buf, income_number);
            Serial.println(buf);
            display_string(2, 0, buf);
            //memcpy(call_label[0].text, buf, strlen(buf));

            clean_btn.text = "Decline";
            dial_btn.text = "Answer";
            }
            break;
        default:
            break;
    }
        
    menu_screen();
    
}

void init_menu()
{

    current_menu = &home_screen;
    root_screen = &home_screen;
    
}
 

void get_vendor_name()
{
    char *ptr = buffer1[0];
    
    memset(buffer1, 0, sizeof(buffer1));
    gsm_send("AT+CGMI\r\n", ptr);

    strcpy(MODULE_INFO.module_vendor, buffer1[1]);
#if 0
    lcd_set_xy(0, 1);
    lcd_string(MODULE_INFO.module_vendor);
#endif
#if 0 
    Serial.println(buffer1[0]);
    Serial.println(buffer1[1]);
    Serial.println(buffer1[2]);
#endif
}

void get_module_id()
{
    char *ptr = buffer1[0];
    
    memset(buffer1, 0, sizeof(buffer1));
    gsm_send("AT+CGMM\r\n", ptr);

    strcpy(MODULE_INFO.module_id, buffer1[1]);

#if 0
    lcd_set_xy(0, 3);
    lcd_string(MODULE_INFO.module_id);
#endif 
    
#if 0
    Serial.println(&buffer1[0][0]);
    Serial.println(&buffer1[1][0]);
    Serial.println(&buffer1[2][0]);
    i = 0;
    while(cgmm[i])
        Serial.println(cgmm[i++], HEX);
#endif
}

void get_module_sn()
{
    char *ptr = buffer1[0];

    memset(buffer1, 0, sizeof(buffer1));
    gsm_send("AT+CGSN\r\n", ptr);

    strcpy(MODULE_INFO.module_sn, buffer1[1]);

#if 0
    lcd_set_xy(0, 4);
    lcd_string(MODULE_INFO.module_sn);
#endif
}

void get_netregist_stat()
{
    char *ptr = buffer1[0];
    
    memset(buffer1, 0, sizeof(buffer1));
    gsm_send("AT+CREG?\r\n", ptr);

    strcpy(MODULE_INFO.module_regist_stat, buffer1[1]);
#if 0
    //lcd_clear_row(4);
    lcd_set_xy(0, 4);
    lcd_string(MODULE_INFO.module_regist_stat);
#endif
}

void get_sim_stat()
{
    char *ptr = buffer1[0];
    
    memset(buffer1, 0, sizeof(buffer1));
    gsm_send("AT%TSIM\r\n", ptr);

    strcpy(MODULE_INFO.module_sim_stat, buffer1[1]);
#if 0
    lcd_set_xy(0, 4);
    lcd_string(MODULE_INFO.module_sim_stat);
#endif
}

static char gsm_operator_state = 0x0;
void get_operator()
{
    char *ptr = buffer1[0];
    unsigned int mode = 0xff, format = 0xff;
    char operator_name[20];
    ElemType elem;
    char tmp[30];
    char ok_flag[3];
    bool flag = false;
    int i;
    char *value_p;

    memset(operator_name, 0, sizeof(operator_name));

    switch(gsm_operator_state){
        case 0x0:
            if(try_lock()){
                memset(buffer1, 0, sizeof(buffer1));
                gsm_send("AT+COPS?\r\n", NULL);
                gsm_operator_state = 0x1;
            }
            break;
        case 0x1:
            if(!cb_isempty(cb)){
                cb_read(cb, &elem);
               
                if(strstr(elem.data, "+COPS") == NULL){
                    debug_prt(elem.data, elem.len);
                    goto FIN;
                }

                memset(tmp, 0, sizeof(tmp));
                memcpy(tmp, elem.data+9, elem.len - 9 - 5);
                tmp[elem.len + 1] = '\0';
                //\r\n+COPS: 0,0,"CHN-CUGSM"\r\nOK\r\n
                sscanf(tmp, "%d,%d,\"%s", &mode, &format, operator_name);
                //Serial.println(operator_name);
                
                // remove the last colon character
                for(i = 0; i < strlen(operator_name); i++){
                    if(operator_name[i] == 0x22)
                        flag = true;
                    if(flag)
                        operator_name[i] = '\0';
                }
            }
FIN:
            unlock();
            gsm_operator_state = 0x0;
            break;
        default:
            break;
    }
    
    if(operator_name){
        strcpy(MODULE_INFO.module_operator, operator_name);
    }else
        strcpy(MODULE_INFO.module_operator, "NoService");

#if 0
    Serial.println(mode);
    Serial.println(format);
    Serial.println(operator_name);
    Serial.println(ok_flag);
#endif

#if 1
    Serial.println(MODULE_INFO.module_operator);
#endif
}

void gsm_sms_format()
{
    ElemType elem;
    char *ptr = buffer1[0];

    gsm_send("AT+CMGF=1\r\n", ptr);
    recv_gsm(NULL);    

    cb_read(cb, &elem);

    Serial.println("helloworld");
    //debug_prt(elem.data, elem.len);

}

void gsm_sendsms()
{
    char *ptr = buffer1[0];
    char buf[50] = {0};
    ElemType elem;
    char end_flag = 0x1a;

    gsm_sms_format();
#if 0
    gsm_send("AT+CMGS=\"13720405960\"\r\n", ptr);
    recv_gsm(NULL);
    cb_read(cb, &elem);
    debug_prt(elem.data, elem.len);
    gsm_send("hello,world", ptr);
    gsm_send(&end_flag, ptr);
    recv_gsm(NULL);
    cb_read(cb, &elem);
    debug_prt(elem.data, elem.len);
#endif 

}

bool recv_lock = false;
int try_lock()
{
    if(recv_lock)
        return 0;
    else
        recv_lock = true;

    return 1;
}

void unlock()
{
    if(recv_lock)
        recv_lock = false;
}

static bool ui_flag = false;
void update_gui_ready()
{
    ui_flag = true; 
}

void update_gui_done()
{
    ui_flag = false;
}

bool get_gui_flag()
{
    return ui_flag;
}

void setup(void)
{
    pinMode(22, INPUT);
    pinMode(23, INPUT);

    // RING pin from GSM
    //pinMode(53, INPUT);
   
    cb_init(cb, 10);

    Serial.begin(9600);
    init_gsm();

    gsm_echo_mode(false);
    
    lcd_init();
    
    memset(&MODULE_INFO, 0, sizeof(struct module_info));
#if 0
    get_module_id();
    get_vendor_name(); 
    get_module_sn();
    //get_sim_stat();
    get_operator();
    get_netregist_stat();
#endif 

    init_menu();

    lcd_clear();
    //main_screen();
    menu_screen();
        
}
bool operator_flag = false;
bool recv_done = false;
char keycode;
ElemType elem;
unsigned int gsm_opr_cnt = 0;
void loop(void)
{
#if 0
    //get_operator();
    //gsm_call_check();
    //gsm_query_call(income_number);
    //gsm_query_me_state();
    //debug_gsm_cmd("AT+CPAS\r\n", 0);
    /*
    keycode = keypad.getKey();
    if(keycode == KEY_1){
    Serial.println("this is seprate line--------");
    //debug_gsm_cmd("ATD;\r\n", 0);
    //flag = true;
    }
    */
    //debug_gsm_cmd("AT+COPS?\r\n", 0);
    //debug_gsm_cmd(NULL, 1);
    delay(100);
    recv_gsm(NULL);
#endif

    if(get_gui_flag()){
        lcd_clear();
        update_gui_done();
        menu_screen();
    }

    delay(10);
    gsm_call_check();
    gsm_query_call(income_number);
    gsm_query_me_state();

    
    if(gsm_opr_cnt++ > 10){
        operator_flag = true;
        gsm_opr_cnt = 0;
    }else
        operator_flag = false;
    
    if(gsm_opr_cnt < 2)
        get_operator();

    keycode = keypad.getKey();
    current_menu->proc(keycode);
    
    recv_gsm(NULL);
        
}
