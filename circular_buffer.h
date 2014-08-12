#ifndef __CIRCULAR_BUFFER__
#define __CIRCULAR_BUFFER__

#define CIRCULAR_BUFFER

typedef struct{
    char data[100];
    unsigned int len;

}ElemType;

typedef struct{
    unsigned int size;
    unsigned int start;
    unsigned int end;
    ElemType *elements;
}Circular_Buffer;


void cb_init(Circular_Buffer *cb, unsigned int size);
void cb_free(Circular_Buffer *cb);
unsigned int cb_isfull(Circular_Buffer *cb);
unsigned int cb_isempty(Circular_Buffer *cb);
void cb_write(Circular_Buffer *cb, ElemType *elem);
void cb_read(Circular_Buffer *cb, ElemType *elem);
#endif
