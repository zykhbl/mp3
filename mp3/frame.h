//
//  frame.h
//  mp3
//
//  Created by zykhbl on 16/3/9.
//  Copyright © 2016年 zykhbl. All rights reserved.
//

#ifndef frame_h
#define frame_h

#include "common.h"

#define T frame
typedef struct T *T;

//MPEG Header Definitions - Mode Values
#define	MPG_MD_STEREO           0
#define	MPG_MD_JOINT_STEREO     1
#define	MPG_MD_DUAL_CHANNEL     2
#define	MPG_MD_MONO             3

#define	SYNC_WORD			(long)0xfff
#define	SYNC_WORD_LENGTH	12

#define	SBLIMIT				32
#define	SSLIMIT				18

typedef struct {//帧头格式:4字节(32位：11111111111...(12个1开头))
    int version;
    int lay;
    int error_protection;
    int bitrate_index;
    int sampling_frequency;
    int padding;
    int extension;
    int mode;
    int mode_ext;
    int copyright;
    int original;
    int emphasis;
} layer;

//Parent Structure Interpreting some Frame Parameters in Header
struct frame {
    layer       header;        //raw header information
    int         actual_mode;    //when writing IS, may forget if 0 chs
    int         stereo;         //1 for mono, 2 for stereo
    int         jsbound;        //first band of joint stereo coding
    int         sblimit;        //total number of sub bands
};

extern T create_frame_struc();

extern int seek_sync(Bit_stream_struc bs, unsigned long sync, int N);
extern void decode_info(Bit_stream_struc bs, T fr_ps);
extern void hdr_to_frps(T fr_ps);
extern void writeHdr(T fr_ps);

extern void buffer_CRC(Bit_stream_struc bs, unsigned int *old_crc);
extern int main_data_slots(frame fr_ps);

#undef T

#endif /* frame_h */
