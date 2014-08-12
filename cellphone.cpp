#include "Arduino.h"
#include "lcd.h"
#include "cellphone.h"
#include "HardwareSerial.h"
#include "gsmlib.h"
#include "circular_buffer.h"
#include "call.h"

#define ARDUINO 150
#include "Keypad.h"

Circular_Buffer *cb;

Keypad keypad = Keypad(makeKeymap(hexkey), row_pins, col_pins, ROWS, COLS);

void home_proc(char keycode);
void sms_proc(char keycode);
void phonebook_proc(char keycode);
void callrecord_proc(char keycode);
void setting_proc(char keycode);
void dial_proc(char keycode);
void sysinfo_proc(char keycode);

struct menuitem phonebook_menu;
struct menuitem callrecord_menu;

struct UILabel sms_label[] = {
    {"SMS", 30, 2,},
};

struct UILabel setting_label[] = {
    {"Setting", 20, 2,},
};

struct UILabel call_label[] = {
    {"", 0, 3,}
};
struct UILabel sysinfo_label[] = {
    {"ModuleInfo", 0, 0},
    {"SysInfo", 0, 1,},
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

struct menuitem *current_menu = &home_screen;
struct menuitem *root_screen = &home_screen;

void get_vendor_name();
void get_operator(char *);
void get_module_sn();
void get_sim_stat();
void get_operator();
void get_module_id();
void get_netregist_stat();

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
    if(keycode == KEY_ENTER){
        current_menu = &sms_menu;
        update_gui_ready();
        //menu_screen();
    }

    
    if(keycode == KEY_CALL){
        current_menu = &dial_screen;
        update_gui_ready();
        //menu_screen();
    }
   
    
    if(keycode == KEY_QUIT){
        current_menu = &home_screen;
        //menu_screen();
    }
        menu_screen();


    if(get_call_state() != END_STATE){
        current_menu = &dial_screen;
        update_gui_ready();
    }
}

void sms_proc(char keycode)
{
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

unsigned int dial_index = 0;
char dial_number[20] = "\0";
bool dial_flag = false;
bool hangup_flag = false;
void dial_proc(char keycode)
{
        
    if(keycode == KEY_QUIT){
        if(dial_flag)
            gsm_hangup();
        //lcd_clear();
        memset(dial_number, '\0', sizeof(dial_number));
        dial_index = 0;
        current_menu = &home_screen;
        update_gui_ready();
        //menu_screen();
    }

    if(keycode == KEY_CALL){
        if(get_call_state() == END_STATE){
        gsm_dial(dial_number);
        dial_flag = true;
        }

        if(get_call_state() == CALL_STATE)
            gsm_call_answer();
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
            call_label[0].text = "Calling";
            break;
        default:
            break;
    }
        
    menu_screen();
    
}

void init_menu()
{
    
}
 

void get_vendor_name()
{
    char *ptr = buffer1[0];
    
    memset(buffer1, 0, sizeof(buffer1));
    at_send("AT+CGMI\r\n", ptr);

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
    at_send("AT+CGMM\r\n", ptr);

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
    at_send("AT+CGSN\r\n", ptr);

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
    at_send("AT+CREG?\r\n", ptr);

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
    at_send("AT%TSIM\r\n", ptr);

    strcpy(MODULE_INFO.module_sim_stat, buffer1[1]);
#if 0
    lcd_set_xy(0, 4);
    lcd_string(MODULE_INFO.module_sim_stat);
#endif
}

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

    memset(buffer1, 0, sizeof(buffer1));
    memset(operator_name, 0, sizeof(operator_name));
    at_send("AT+COPS?\r\n", NULL);
   
    //recv_gsm(NULL);
    if(!cb_isempty(cb)){
        cb_read(cb, &elem);

        memset(tmp, 0, sizeof(tmp));
        memcpy(tmp, elem.data+14, elem.len-14);
        tmp[elem.len + 1] = '\0';
        //Serial.println(elem.data);

        //AT+COPS?+COPS: 0,0,"CHN-CUGSM"OK
        sscanf(tmp, "%d,%d,\"%s", &mode, &format, operator_name);
        for(i = 0; i < strlen(operator_name); i++){
        if(operator_name[i] == 0x22)
            flag = true;
        if(flag)
            operator_name[i] = '\0';
        }
            
#if 0
    Serial.println(mode);
    Serial.println(format);
    Serial.println(operator_name);
    Serial.println(ok_flag);
#endif

    if(format != 0xff){
        strcpy(MODULE_INFO.module_operator, operator_name);
    }else
        strcpy(MODULE_INFO.module_operator, "NULL");
#if 0
    Serial.println(buffer1[0]);
    Serial.println(buffer1[1]);
    Serial.println(buffer1[2]);
    Serial.println(MODULE_INFO.module_operator);
#endif
    }
}

void gsm_rtc()
{
    ElemType elem;
    char buf[30];
    char tmp[30];
    
    strcpy(buf, "AT+CCLK?\r\n");
    at_send(buf, NULL);

    //recv_gsm(NULL);
    if(cb_isempty(cb))
        return;

        cb_read(cb, &elem);
        Serial.println(elem.data);
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

void gsm_csq()
{
    char *ptr = buffer1[0];
    unsigned int rssi = 0xff, ber = 0xff;
    char operator_name[20];

    memset(buffer1, 0, sizeof(buffer1));
    memset(operator_name, 0, sizeof(operator_name));
    at_send("AT+CSQ\r\n", ptr);

    
    sscanf(buffer1[1], "+CSQ: %d,%d", &rssi, &ber);
    if(rssi != 0xff){
        MODULE_INFO.module_csq = (rssi < 8) | ber;
    }else
        MODULE_INFO.module_csq = 0;
#if 0
    Serial.println(buffer1[0]);
    Serial.println(buffer1[1]);
    Serial.println(buffer1[2]);
    Serial.println(MODULE_INFO.module_operator);
#endif


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
   
    cb_init(cb, 50);

    Serial.begin(9600);
    init_gsm();

    //gsm_echo_mode(false);
    
    lcd_init();
#if 0  
    memset(buffer1, 0, sizeof(buffer1));
    exec_atcommand("ATE0\r\n", NULL);
#endif 

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

bool recv_done = false;
char keycode;
ElemType elem;
unsigned int gsm_opr_cnt = 0;
void loop(void)
{
    delay(20);
#if 1
    if(get_gui_flag()){
        lcd_clear();
        update_gui_done();
        menu_screen();
    }
#endif 
        recv_gsm(NULL);
#if 1
    gsm_opr_cnt++;
    if(gsm_opr_cnt > 100){
        get_operator();
        gsm_opr_cnt = 0;
    }
#endif
    
    keycode = keypad.getKey();
    current_menu->proc(keycode);
        
}
