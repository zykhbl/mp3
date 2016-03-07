//
//  common.h
//  mp3Decoder
//
//  Created by zykhbl on 16/2/24.
//  Copyright © 2016年 zykhbl. All rights reserved.
//

#ifndef common_h
#define common_h

#include <stdio.h>

//MPEG Header Definitions - Mode Values
#define	MPG_MD_STEREO           0
#define	MPG_MD_JOINT_STEREO     1
#define	MPG_MD_DUAL_CHANNEL     2
#define	MPG_MD_MONO             3

#define	SYNC_WORD			(long)0xfff
#define	SYNC_WORD_LENGTH	12

#define	ALIGNING			8

#define	MINIMUM				4  // Minimum size of the buffer in bytes
#define	MAX_LENGTH			32 //Maximum length of word written or read from bit stream

#define	BINARY				0  //Binary input file
#define	READ_MODE			0  //Decode mode only

#define	FALSE				0
#define	TRUE				1

#define	MIN(A, B)			((A) < (B) ? (A) : (B))

#define	SBLIMIT				32
#define	SSLIMIT				18
#define	BUFFER_SIZE			4096
#define	HAN_SIZE			512
#define	SCALE				32768

#define	PI					3.14159265358979
#define	PI64				PI/64
#define	PI4					PI/4

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
} layer, *the_layer;


//Parent Structure Interpreting some Frame Parameters in Header
typedef struct {
    layer       *header;        //raw header information
    int         actual_mode;    //when writing IS, may forget if 0 chs
    int         stereo;         //1 for mono, 2 for stereo
    int         jsbound;        //first band of joint stereo coding
    int         sblimit;        //total number of sub bands
} frame_params;

typedef struct  bit_stream_struc {
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
} Bit_stream_struc;

//Layer III side information
typedef struct {
    unsigned main_data_begin;
    unsigned private_bits;
    struct {
        unsigned scfsi[4];
        struct gr_info_s {
            unsigned part2_3_length;
            unsigned big_values;
            unsigned global_gain;
            unsigned scalefac_compress;
            unsigned window_switching_flag;
            unsigned block_type;
            unsigned mixed_block_flag;
            unsigned table_select[3];
            unsigned subblock_gain[3];
            unsigned region0_count;
            unsigned region1_count;
            unsigned preflag;
            unsigned scalefac_scale;
            unsigned count1table_select;
        } gr[2];
    } ch[2];
} III_side_info_t;

typedef struct {
    int l[23];			//[cb]
    int s[3][13];		//[window][cb]
} III_scalefac_t[2];	//[ch]

FILE *openTableFile(char *name);

void writeHdr(frame_params *fr_ps);

void *mem_alloc(unsigned long block, char *item);
void alloc_buffer(Bit_stream_struc *bs, int size);
void desalloc_buffer(Bit_stream_struc *bs);

void open_bit_stream_r(Bit_stream_struc *bs, char *bs_filenam, int size);
void close_bit_stream_r(Bit_stream_struc *bs);
int	end_bs(Bit_stream_struc *bs);
unsigned long sstell(Bit_stream_struc *bs);
void refill_buffer(Bit_stream_struc *bs);

unsigned int get1bit(Bit_stream_struc *bs);
unsigned long getbits(Bit_stream_struc *bs, int N);
int seek_sync(Bit_stream_struc *bs, unsigned long sync, int N);

int js_bound(int lay, int m_ext);
void hdr_to_frps(frame_params *fr_ps);

void hputbuf(unsigned int val, int N);
unsigned long hsstell(); 
unsigned long hgetbits(int N); 
unsigned int hget1bit(); 
void rewindNbits(int N); 
void rewindNbytes(int N);

#endif
