//
//  common.h
//  mp3Decoder
//
//  Created by zykhbl on 16/2/24.
//  Copyright © 2016年 zykhbl. All rights reserved.
//

#ifndef common_h
#define common_h

#define T Bit_stream_struc
typedef struct T *T;

#define	FALSE				0
#define	TRUE				1

extern void *mem_alloc(unsigned long block, char *item);

extern T create_Bit_stream_struc();
extern void open_bit_stream_r(T bs, char *bs_filenam, int size);
extern void close_bit_stream_r(T bs);
extern int	end_bs(T bs);
extern unsigned long sstell(T bs);

extern unsigned int get1bit(T bs);
extern unsigned long getbits(T bs, int N);

extern void hputbuf(unsigned int val, int N);
extern unsigned long hsstell();
extern unsigned long hgetbits(int N);
extern unsigned int hget1bit();
extern void rewindNbits(int N);
extern void rewindNbytes(int N);

#undef T

#endif
