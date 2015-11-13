#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <malloc.h>
extern "C"{
#include "x264.h"
}
#include "encoder.h"

int x264_width=640;
int x264_height=480;
int fps=25;
size_t yuv_size=640*480*3/2;
x264_t *encoder;
x264_picture_t pic_in,pic_out;
int inf=0,outf=0;
unsigned char *yuv_buffer=NULL;
x264_param_t param;
long int i_pts=0;


int open_x264file(const char *dst_filename_420)
{
	inf=open(dst_filename_420,O_RDONLY);
	
	if(inf<0)
	{
		printf("dst_file open failed!\n");
		return -1;
	}
	
	outf=open("test.h264",O_CREAT|O_WRONLY,444);
	
	if(outf<0)
	{
		printf("test.h264 open  failed!\n");
		return -1;

	}

}


void init_encoder(void)
{

	x264_param_default_preset(&param, "veryfast", "zerolatency");
	param.i_threads=1;
	param.i_width=x264_width;
	param.i_height=x264_height;
	param.i_fps_num=fps;
	param.i_fps_den=1;

	param.i_keyint_max=25;
	param.b_intra_refresh=1;
	//If param->b_annexb is set, Annex-B bytestream with startcode.
	//* This size is the size used in mp4/similar muxing; it is equal to ( i_payload-4 ) --> uint8_t *p_payload;
	param.b_annexb=1;   
	x264_param_apply_profile(&param,"baseline");
	encoder=x264_encoder_open(&param);
	printf("in  init_encoder function\n");

}


void init_picture(size_t yuv_size)
{
	x264_picture_alloc(&pic_in, X264_CSP_I420, x264_width, x264_height);
	yuv_buffer=(unsigned char *)malloc(yuv_size);


	pic_in.img.plane[0]=yuv_buffer;// Y--component
	pic_in.img.plane[1]=pic_in.img.plane[0]+x264_width*x264_height;//U--component
	pic_in.img.plane[2]=pic_in.img.plane[1]+x264_width*x264_height/4;//V--component

	printf("in init_picture function\n");

	//return yuv_buffer;
}


void encode_frame(void)
{
	x264_nal_t *nals,*nal;
	int nnal;

	while(read(inf,yuv_buffer,yuv_size)>0)
	{
		pic_in.i_pts=i_pts++;
		x264_encoder_encode(encoder,&nals,&nnal,&pic_in,&pic_out);
		
		for(nal=nals;nal<nals+nnal;nal++)
		{
			write(outf,nal->p_payload,nal->i_payload);
		}
	}
	i_pts=0;

}


void close_encoder(void)
{
	x264_encoder_close(encoder);
	// close(inf);
	// close(outf);
	free(yuv_buffer);
}







