//
//  decode.h
//  mp3Decoder
//
//  Created by zykhbl on 16/2/24.
//  Copyright © 2016年 zykhbl. All rights reserved.
//

#ifndef decode_h
#define decode_h

#include "common.h"

//MPEG Header Definitions - Mode Values
#define	MPG_MD_STEREO           0
#define	MPG_MD_JOINT_STEREO     1
#define	MPG_MD_DUAL_CHANNEL     2
#define	MPG_MD_MONO             3

#define	SYNC_WORD			(long)0xfff
#define	SYNC_WORD_LENGTH	12

#define	ALIGNING			8

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

int seek_sync(Bit_stream_struc bs, unsigned long sync, int N);

void writeHdr(frame_params *fr_ps);
void hdr_to_frps(frame_params *fr_ps);

void decode_info(Bit_stream_struc, frame_params*);
void III_get_side_info(Bit_stream_struc bs, III_side_info_t *si, frame_params *fr_ps);
void III_get_scale_factors(III_scalefac_t *scalefac, III_side_info_t *si, int gr, int ch, frame_params *fr_ps);

void initialize_huffman();
void III_hufman_decode(long int is[SBLIMIT][SSLIMIT], III_side_info_t *si, int ch, int gr, int part2_start, frame_params *fr_ps);

void III_dequantize_sample(long int[SBLIMIT][SSLIMIT], double [SBLIMIT][SSLIMIT], III_scalefac_t *, struct gr_info_s *, int, frame_params *);
void III_reorder(double xr[SBLIMIT][SSLIMIT], double ro[SBLIMIT][SSLIMIT], struct gr_info_s *gr_info, frame_params *fr_ps);
void III_stereo(double xr[2][SBLIMIT][SSLIMIT], double lr[2][SBLIMIT][SSLIMIT], III_scalefac_t *scalefac, struct gr_info_s *gr_info, frame_params *fr_ps);
void III_antialias(double xr[SBLIMIT][SSLIMIT], double hybridIn[SBLIMIT][SSLIMIT], struct gr_info_s *gr_info, frame_params *fr_ps);

void inv_mdct(double in[18], double out[36], int block_type);
void III_hybrid(double fsIn[SSLIMIT], double tsOut[SSLIMIT], int sb, int ch, struct gr_info_s *gr_info, frame_params *fr_ps);

void create_syn_filter(double filter[64][SBLIMIT]);
void read_syn_window(double window[HAN_SIZE]);
int subBandSynthesis (double *bandPtr, int channel, short *samples);

void out_fifo(short pcm_sample[2][SSLIMIT][SBLIMIT], int num, frame_params *fr_ps, int done, FILE *outFile, unsigned long *psampFrames);
void buffer_CRC(Bit_stream_struc bs, unsigned int *old_crc);
int main_data_slots(frame_params fr_ps);

#endif
