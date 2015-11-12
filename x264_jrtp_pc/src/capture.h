//#include <stdio.h>  
//#include <stdlib.h>  
//#include <string.h>  
//#include <assert.h>  
  
//#include <getopt.h>             /* getopt_long() */  
#ifndef CAPTURE_H
#define CAPTURE_H
#ifdef __cplusplus
extern "C"
{
#endif
#define CLEAR(x) memset (&(x), 0, sizeof (x))  
  
typedef enum {  
    IO_METHOD_READ, IO_METHOD_MMAP, IO_METHOD_USERPTR,  
} io_method;  
  
struct buffer {  
    void * start;  
    size_t length;  
};  
  
extern char * dev_name ; 
extern int frame_num; 
extern io_method io ;  
extern int fd ;  
extern struct buffer * buffers;  
extern unsigned int n_buffers ;  
  
extern FILE *fp;  
extern const char *filename; 

//typedef unsigned char uint8_t;
//uint8_t *src_data[4],*dst_data[4];
//int src_linesize[4],dst_linesize[4];
//int src_width=640;
//int src_height=480;
//int dst_width,dst_height;

//enum AVPixelFormat src_pix_fmt=AV_PIX_FMT_YUYV422;
//enum AVPixelFormat dst_pix_fmt=AV_PIX_FMT_YUV420P;

//const char *dst_size="640x480";
//const char *dst_filename="frame_yuyv420.yuv";
//char *src_filename;//="test.yuv";

//FILE *dst_file=NULL,*src_file=NULL;

//int dst_bufsize;
//int src_bufsize;

//struct SwsContext *sws_ctx;
//int ret;
extern FILE *fpYUYV ;
extern FILE *fpYUV420 ;
extern FILE *fpH264 ;

void openFiles(void);

void errno_exit(const char * s);
int xioctl(int fd, int request, void * arg);
void process_image(const void * p, int size);
int read_frame(void);
void mainloop(int frame_num);
void stop_capturing(void);
void start_capturing(void);
void uninit_device(void);
void init_read(unsigned int buffer_size);
void init_mmap(void);
void init_userp(unsigned int buffer_size);
void init_device(void);
void close_device(void);
void open_device(void);
#ifdef __cplusplus
}
//void open_yuvfile(const char *filename);
//void alloc_image(void);
//void swscale_start(void);
//void swscale_close(void);
#endif
#endif

