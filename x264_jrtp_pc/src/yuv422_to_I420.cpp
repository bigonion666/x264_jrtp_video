#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
//#ifdef __cplusplus
//extern "C" {
//#endif
//#define __STDC_CONSTANT_MACROS
//#ifdef _STDINT_H
//#undef _STDINT_H
//#endif
//#include<stdint.h>
//#endif
//#include "libavutil/imgutils.h"
//#include "libavutil/parseutils.h"
//#include "libavutil/pixfmt.h"
//#include "libswscale/swscale.h"
//#ifdef __cplusplus
//}
//#endif

#include "yuv422_to_I420.h"


//typedef unsigned char uint8_t;
//int frame_num=0;

//const char *filename = "test.yuv\0"; 

unsigned char *src_data[4],*dst_data[4];
int src_linesize[4],dst_linesize[4];
int src_width=640;
int src_height=480;
int dst_width=640;
int dst_height=640;

enum AVPixelFormat src_pix_fmt=AV_PIX_FMT_YUYV422;
enum AVPixelFormat dst_pix_fmt=AV_PIX_FMT_YUV420P;

const char *dst_size="640x480";
const char *dst_filename="frame_yuyv420.yuv";


FILE *dst_file=NULL,*src_file=NULL;

int dst_bufsize;
int src_bufsize;

struct SwsContext *sws_ctx;
int ret=-1;

unsigned char buffI420p[640*480*3/2];
const int I420size = 460800;


//int main(){
void open_yuvfile(const char *filename)
{
    if(av_parse_video_size(&dst_width,&dst_height,dst_size)<0){
        fprintf(stderr,"Invalid size:'%s'\n",dst_size);
        exit(1);
    }

    src_file=fopen(filename,"rb");

    if(!src_file){
        fprintf(stderr,"could not open source file %s\n",filename);
        exit(1);
    }

    dst_file=fopen(dst_filename,"wb");

    if(!dst_file){
        fprintf(stderr,"could not open dst file %s\n",dst_filename);
        exit(1);
    }
}

void swscale_close(void)
{
    av_freep(&src_data[0]);
    av_freep(&dst_data[0]);
    sws_freeContext(sws_ctx);
    fclose(src_file);
    fclose(dst_file);

//return 0;
}

void alloc_image(void)
{
    printf("Alloc image Start\n");
    sws_ctx=sws_getContext(src_width,src_height,src_pix_fmt,dst_width,dst_height,dst_pix_fmt,SWS_BILINEAR,NULL,NULL,NULL);

    if(!sws_ctx){
        fprintf(stderr,"impossible to create scale context for the conversion\n");
        ret=AVERROR(EINVAL);
        //goto end;
        swscale_close();
    }

    if((ret=av_image_alloc(src_data,src_linesize,src_width,src_height,src_pix_fmt,16))<0){
        fprintf(stderr,"could not allocate source image\n");
        //goto end;
        swscale_close();
    }

    src_bufsize=ret;

    if((ret=av_image_alloc(dst_data,dst_linesize,dst_width,dst_height,dst_pix_fmt,1))<0){
        fprintf(stderr,"could not allocate dst image\n");
        //goto end;
        swscale_close();
    }
    dst_bufsize=ret;
}

void swscale_start(void)
{
    printf("Swscale Start\n");
    while(fread(src_data[0],1,src_bufsize,src_file)==src_bufsize){
        sws_scale(sws_ctx,(const uint8_t * const*)src_data,src_linesize,0,src_height,dst_data,dst_linesize);
        fwrite(dst_data[0],1,dst_bufsize,dst_file);
    }
    fprintf(stderr,"Scaling succeeded!\n");
}


int YUV422To420x264(unsigned char *yuv422, unsigned char *yuv420, int width, int height)
{

       int ynum=width*height;
    //printf("in the YUV422To420x264 function %d\n",ynum );
       int i,j,k=0;
       //得到Y分量
      // printf("Y COMPONENT\n");
       for(i=0;i<ynum;i++){
                //printf("%d\n",i );
               yuv420[i]=yuv422[i*2];
       }
       //printf("%d\n",i );
      // qDebug()<<"outLength";
       //得到U分量
       //printf("U COMPONENT\n");
       for(i=0;i<height;i++){
        //printf("%d\n",i );
               if((i%2)!=0)continue;//只取其中奇数行
               for(j=0;j<(width/2);j++){
                       if((4*j+1)>(2*width))break;
                       yuv420[ynum+k*2*width/4+j]=yuv422[i*2*width+4*j+1];
               }
               k++;
       }
       k=0;
       //得到V分量
       //printf("V COMPONENT\n");
       for(i=0;i<height;i++){
               if((i%2)==0)continue;
               for(j=0;j<(width/2);j++){
                       if((4*j+3)>(2*width))break;
                       yuv420[ynum+ynum/4+k*2*width/4+j]=yuv422[i*2*width+4*j+3];

               }
               k++;
       }

       return 1;
}
