//
//  wav.h
//  mp3Decoder
//
//  Created by zykhbl on 16-2-28.
//  Copyright (c) 2016年 zykhbl. All rights reserved.
//

#ifndef mp3Decoder_wav_h
#define mp3Decoder_wav_h

#define T wav
typedef struct T *T;

extern void writeWAVHeader(FILE *f);

#undef T

#endif
