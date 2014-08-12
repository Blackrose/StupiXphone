#ifndef CELLPHONE_H
#define CELLPHONE_H


#define KEY_ENTER   'e'
#define KEY_QUIT    'q'
#define KEY_CALL    'c'
#define KEY_HOLD    'h'
#define KEY_UP      'u'
#define KEY_DOWN    'd'
#define KEY_LEFT    'l'
#define KEY_RIGHT   'r'
#define KEY_1       '1'
#define KEY_2       '2'
#define KEY_3       '3'
#define KEY_4       '4'
#define KEY_5       '5'
#define KEY_6       '6'
#define KEY_7       '7'
#define KEY_8       '8'
#define KEY_9       '9'
#define KEY_0       '0'
#define KEY_STAR    '*'
#define KEY_HASH    '#'

#define ROWS 7
#define COLS 3

char hexkey[ROWS][COLS] = {
    {'e', 'u', 'q'},
    {'l', ' ', 'r'},
    {'c', 'd', 'h'},
    {'1', '2', '3'},
    {'4', '5', '6'},
    {'7', '8', '9'},
    {'*', '0', '#'}
};

byte row_pins[ROWS] = {25, 26, 27, 28, 29, 30, 31};
byte col_pins[COLS] = {22, 23, 24};


struct module_info{
    char module_id[20];    
    char module_vendor[20];    
    char module_sn[30];
    char module_regist_stat[30];
    char module_sim_stat[30];
    char module_operator[30];
    unsigned int module_csq;
}MODULE_INFO;


#define ATBUFF_CNT 10
#define ATBUFF_SZ 100
char buffer1[ATBUFF_CNT][ATBUFF_SZ];

struct UILabel{
    char *text;
    unsigned int pos_x;
    unsigned int pos_y;
};

struct menuitem{
    unsigned int name;
    struct UILabel *label;
    unsigned int statusbar_enable;
    struct menuitem *left;
    struct menuitem *right;
    void (*proc)(char);
    struct UILabel *left_btn;
    struct UILabel *right_btn;
    unsigned label_cnt;
};

enum screen_name{
    HOME_SCN = 1,
    SMS_SCN,
    DIAL_SCN,
    SETTING_SCN,
    SYSINFO_SCN,
};

#endif
