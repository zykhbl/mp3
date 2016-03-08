//
//  common.c
//  mp3Decoder
//
//  Created by zykhbl on 16/2/24.
//  Copyright © 2016年 zykhbl. All rights reserved.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "common.h"

#define	MINIMUM				4  // Minimum size of the buffer in bytes
#define	MAX_LENGTH			32 //Maximum length of word written or read from bit stream

#define	BINARY				0  //Binary input file
#define	READ_MODE			0  //Decode mode only

#define	MIN(A, B)			((A) < (B) ? (A) : (B))

#define T Bit_stream_struc

struct T {
    FILE            *pt;            //pointer to bit stream device
    unsigned char   *buf;           //bit stream buffer
    int             buf_size;       //size of buffer (in number of bytes)
    long            totbit;         //bit counter of bit stream
    int             buf_byte_idx;   //pointer to top byte in buffer
    int             buf_bit_idx;    //pointer to top bit of top byte in buffer
    int             mode;           //bit stream open in read or write mode
    int             eob;            //end of buffer index
    int             eobs;           //end of bit stream flag
    char            format;         //format of file in rd mode (BINARY/ASCII)
};

void *mem_alloc(unsigned long block, char *item) {
    void *ptr;
    ptr = (void *)malloc((unsigned long)block);
    if (ptr != NULL) {
        memset(ptr, 0, block);
    } else {
        printf("Unable to allocate %s\n", item);
        exit(0);
    }
    return ptr;
}

//open and initialize the buffer
void alloc_buffer(T bs, int size) {
    bs->buf = (unsigned char *) mem_alloc(size * sizeof(unsigned char), "buffer");
    bs->buf_size = size;
}

void desalloc_buffer(T bs) {
    free(bs->buf);
}

T create_Bit_stream_struc() {
    T t;
    t = (T)mem_alloc((long) sizeof(*t), "Bit_stream_struc");
    
    return t;
}

//open the device to read the bit stream from it
void open_bit_stream_r(T bs, char *bs_filenam, int size) {
    if ((bs->pt = fopen(bs_filenam, "rb")) == NULL) {
        printf("Could not find \"%s\".\n", bs_filenam);
        exit(1);
    }

    alloc_buffer(bs, size);
    bs->buf_byte_idx = 0;
    bs->buf_bit_idx = 0;
    bs->totbit = 0;
    bs->mode = READ_MODE;
    bs->eob = FALSE;
    bs->eobs = FALSE;
    bs->format = BINARY;
}

void close_bit_stream_r(T bs) {
    fclose(bs->pt);
    desalloc_buffer(bs);
}

//return the status of the bit stream
//returns 1 if end of bit stream was reached
//returns 0 if end of bit stream was not reached
int end_bs(T bs) {
    return(bs->eobs);
}

//return the current bit stream length (in bits)
unsigned long sstell(T bs) {
    return(bs->totbit);
}

void refill_buffer(T bs) {
    register int i = bs->buf_size - 2 - bs->buf_byte_idx;
    register unsigned long n = 1;
    
    while ((i >= 0) && (!bs->eob)) {
        n = fread(&bs->buf[i--], sizeof(unsigned char), 1, bs->pt);
        if (!n) {
            bs->eob = i + 1;
        }
    }
}

int mask[8] = {0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80};

//read 1 bit from the bit stream
unsigned int get1bit(T bs) {
    unsigned int bit;
    register int i;
    
    bs->totbit++;
    
    if (!bs->buf_bit_idx) {
        bs->buf_bit_idx = 8;
        bs->buf_byte_idx--;
        if ((bs->buf_byte_idx < MINIMUM) || (bs->buf_byte_idx < bs->eob)) {
            if (bs->eob) {
                bs->eobs = TRUE;
            } else {
                for (i = bs->buf_byte_idx; i >= 0; i--) {
                    bs->buf[bs->buf_size - 1 - bs->buf_byte_idx + i] = bs->buf[i];
                }
                refill_buffer(bs);
                bs->buf_byte_idx = bs->buf_size - 1;
            }
        }
    }
    bit = bs->buf[bs->buf_byte_idx] & mask[bs->buf_bit_idx - 1];
    bit = bit >> (bs->buf_bit_idx - 1);
    bs->buf_bit_idx--;
    return(bit);
}

int putmask[9] = {0x0, 0x1, 0x3, 0x7, 0xf, 0x1f, 0x3f, 0x7f, 0xff};

//read N bit from the bit stream
unsigned long getbits(T bs, int N) {
    unsigned long val = 0;
    register int i;
    register int j = N;
    register int k, tmp;
    
    if (N > MAX_LENGTH) {
        printf("Cannot read or write more than %d bits at a time.\n", MAX_LENGTH);
    }
    
    bs->totbit += N;
    while (j > 0) {
        if (!bs->buf_bit_idx) {
            bs->buf_bit_idx = 8;
            bs->buf_byte_idx--;
            if ((bs->buf_byte_idx < MINIMUM) || (bs->buf_byte_idx < bs->eob)) {
                if (bs->eob) {
                    bs->eobs = TRUE;
                } else {
                    for (i = bs->buf_byte_idx; i >= 0; i--) {
                        bs->buf[bs->buf_size - 1 - bs->buf_byte_idx + i] = bs->buf[i];
                    }
                    refill_buffer(bs);
                    bs->buf_byte_idx = bs->buf_size - 1;
                }
            }
        }
        k = MIN(j, bs->buf_bit_idx);
        tmp = bs->buf[bs->buf_byte_idx] & putmask[bs->buf_bit_idx];
        tmp = tmp >> (bs->buf_bit_idx - k);
        val |= tmp << (j - k);
        bs->buf_bit_idx -= k;
        j -= k;
    }
    return val;
}

#define BUFSIZE 4096
static unsigned int buf[BUFSIZE];
static unsigned int buf_bit_idx = 8;
static unsigned long offset, totbit = 0, buf_byte_idx = 0;

void hputbuf(unsigned int val, int N) {
    if (N != 8) {
        printf("Not Supported yet!!\n");
        exit(-3);
    }
    buf[offset % BUFSIZE] = val;
    offset++;
}

//return the current bit stream length (in bits)
unsigned long hsstell() {
    return totbit;
}

//read N bit from the bit stream
unsigned long hgetbits(int N) {
    unsigned long val = 0;
    register int j = N;
    register int k, tmp;
    
    if (N > MAX_LENGTH) {
        printf("Cannot read or write more than %d bits at a time.\n", MAX_LENGTH);
    }

    totbit += N;
    while (j > 0) {
        if (!buf_bit_idx) {
            buf_bit_idx = 8;
            buf_byte_idx++;
            if (buf_byte_idx > offset) {
                printf("Buffer overflow !!\n");
                exit(3);
            }
        }
        k = MIN(j, buf_bit_idx);
        tmp = buf[buf_byte_idx % BUFSIZE] & putmask[buf_bit_idx];
        tmp = tmp >> (buf_bit_idx - k);
        val |= tmp << (j - k);
        buf_bit_idx -= k;
        j -= k;
    }
    return val;
}

unsigned int hget1bit() {
    return (unsigned int)hgetbits(1);
}

void rewindNbits(int N) {
    totbit -= N;
    buf_bit_idx += N;
    while(buf_bit_idx >= 8){
        buf_bit_idx -= 8;
        buf_byte_idx--;
    }
}

void rewindNbytes(int N) {
    totbit -= N * 8;
    buf_byte_idx -= N;
}

#undef T
