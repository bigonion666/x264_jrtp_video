#ifndef YUV422_TO_I420_H
#define YUV422_TO_I420_H

#ifdef __cplusplus
extern "C" {
#endif
#define __STDC_CONSTANT_MACROS
#ifdef _STDINT_H
#undef _STDINT_H
#endif
#include<stdint.h>
#include "libavutil/imgutils.h"
#include "libavutil/parseutils.h"
#include "libavutil/pixfmt.h"
#include "libswscale/swscale.h"


extern unsigned char *src_data[4],*dst_data[4];
extern int src_linesize[4],dst_linesize[4];
extern int src_width;//=640;
extern int src_height;//t=480;
extern int dst_width,dst_height;

extern enum AVPixelFormat src_pix_fmt;//=AV_PIX_FMT_YUYV422;
extern enum AVPixelFormat dst_pix_fmt;//=AV_PIX_FMT_YUV420P;

extern const char *dst_size;//="640x480";
extern const char *dst_filename;//="frame_yuyv420.yuv";

extern FILE *dst_file,*src_file;

extern int dst_bufsize;
extern int src_bufsize;

extern struct SwsContext *sws_ctx;
extern int ret;

extern unsigned char buffI420p[640*480*3/2];
extern const int I420size;

void open_yuvfile(const char *filename);
void swscale_close(void);
void alloc_image(void);
void swscale_start(void);

int YUV422To420x264(unsigned char *yuv422, unsigned char *yuv420, int width, int height);

#ifdef __cplusplus
}
#endif
#endif
