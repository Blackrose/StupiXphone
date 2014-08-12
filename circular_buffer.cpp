#include <Arduino.h>
#include "circular_buffer.h"

void cb_init(Circular_Buffer *cb, unsigned int size)
{
    cb->size = size + 1;
    cb->start = 0;
    cb->end = 0;
    
    cb->elements = (ElemType *)calloc(size, sizeof(ElemType));
}

void cb_free(Circular_Buffer *cb)
{
    free(cb->elements);
}

unsigned int cb_isfull(Circular_Buffer *cb)
{
    //return (cb->end + 1) % cb->size == cb->start;
    return cb->end == (cb->start ^ cb->size);
}

/* --------------------------------*/
/**
 * @Synopsis :
 *
 * @Param cb : circular buffer which checked
 *
 * @Returns : if empty return 1, otherwise return 0.
 */
/* --------------------------------*/
unsigned int cb_isempty(Circular_Buffer *cb)
{
    //printf("%s:%d,%d\n", __FUNCTION__, cb->end, cb->start);
    return cb->end == cb->start;
}

int cbIncr(Circular_Buffer *cb, int p)
{
    return (p + 1)&(2*cb->size-1);
}

void cb_write(Circular_Buffer *cb, ElemType *elem)
{
#if 1
    cb->elements[cb->end] = *elem;
    cb->end = (cb->end + 1) % cb->size;

    if(cb->start == cb->end)
        cb->start = (cb->start + 1) % cb->size;
#else
    cb->elements[cb->end&(cb->size-1)] = *elem;
    if(cb_isfull(cb)) /* full, overwrite moves start pointer */
        cb->start = cbIncr(cb, cb->start);
    cb->end = cbIncr(cb, cb->end);
#endif
}

void cb_read(Circular_Buffer *cb, ElemType *elem)
{
#if 1
    //memcpy(elem, &cb->elements[cb->start], sizeof(struct CAN_FRAME));
    *elem = cb->elements[cb->start];
    cb->start = (cb->start + 1) % cb->size;
#else
    *elem = cb->elements[cb->start&(cb->size-1)];
    cb->start = cbIncr(cb, cb->start);
#endif

}
