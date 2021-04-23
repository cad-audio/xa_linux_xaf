/*
* Copyright (c) 2015-2020 Cadence Design Systems Inc.
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "circularbuffer.h"

/*******************************************************************************
 * Code
 ******************************************************************************/

circular_buf_t *circularbuf_create(uint32_t size)
{
    circular_buf_t *cb;
	cb = malloc(sizeof(struct circular_buf));
    if (!cb)
    	return NULL;

    cb->head = 0;
    cb->tail = 0;
    cb->filled  = 0;
    cb->max = size;
    cb->buffer  = malloc(size);
    if (!cb->buffer)
    {
        free(cb);
        return NULL;
    }

    return cb;	
}


void circularbuf_destroy(circular_buf_t *cb)
{
    free(cb->buffer);
    free(cb);
}

void circularbuf_clear(circular_buf_t *cb)
{
    cb->head = 0;
    cb->tail = 0;
    cb->filled  = 0;
}

uint32_t circularbuf_get_filled(circular_buf_t *cb)
{
    return cb->filled;
}

uint32_t circularbuf_write(circular_buf_t *cb, uint8_t *data, uint32_t size)
{
    uint32_t remaining = cb->max - cb->tail;


    if (size > (cb->max - cb->filled))
    {
        /* Overflow - update size with availavle bytes*/
        size = cb->max - cb->filled;
    }

    /* Split write into two if it will overflow. */
    if (size > remaining)
    {
        /* First copy to end of buffer. */
        memcpy(cb->buffer + cb->tail, data, remaining);
        /* Second copy from start of buffer, remaining size. */
        memcpy(cb->buffer, data + remaining, size - remaining);
        /* Set write point to 0 + <second copy size>. */
        cb->tail = size - remaining;
    }
    else
    {
        memcpy(cb->buffer + cb->tail, data, size);
        cb->tail += size;
    }

    cb->filled += size;

      return size;
}


uint32_t circularbuf_read(circular_buf_t *cb, uint8_t *data, uint32_t size)
{
   uint32_t remaining = cb->max - cb->head;

    if (size > cb->filled)
    {
        /* Underrun - read vailable bytes. */
        size = cb->filled;
    }

    if (size > remaining)
    {
        if (data)
        {
            /* First copy to end of buffer. */
            memcpy(data, cb->buffer + cb->head, remaining);
            /* Second copy from start of buffer, remaining size. */
            memcpy(data + remaining, cb->buffer, size - remaining);
        }

        cb->head = size - remaining;

    }
    else
    {
        if (data)
        {
            memcpy(data, cb->buffer + cb->head, size);
        }
        cb->head += size;

    }

    cb->filled -= size;
    return size;    
}


