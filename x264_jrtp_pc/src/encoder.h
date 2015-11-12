#ifndef ENCODER_H
#define ENCODER_H
#ifdef __cplusplus
extern "C"
{
#endif
#include <stdint.h>
#include "x264.h"

//extern typedef unsigned char uint8_t;

extern int x264_width;//=640;
extern int x264_height;//=480;
extern int fps;//=25;
extern size_t yuv_size;//=640*480*3/2;
extern x264_t *encoder;
extern x264_picture_t pic_in,pic_out;
extern int inf,outf;
extern unsigned char *yuv_buffer;
extern x264_param_t param;
extern long int i_pts;//=0;

int open_x264file(const char *dst_filename);
void init_encoder(void);
void init_picture(size_t yuv_size);
void encode_frame(void);
void close_encoder(void);



#ifdef __cplusplus
}
#endif
#endif
