/* 
 *  V4L2 video capture example 
 * 
 *  This program can be used and distributed without restrictions. 
 */  
  
#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <assert.h>  
  
#include <getopt.h>             /* getopt_long() */  
  
#include <fcntl.h>              /* low-level i/o */  
#include <unistd.h>  
#include <errno.h>  
#include <malloc.h>  
#include <sys/stat.h>  
#include <sys/types.h>  
#include <sys/time.h>  
#include <sys/mman.h>  
#include <sys/ioctl.h>  
  
#include <asm/types.h>          /* for videodev2.h */  
  
#include <linux/videodev2.h>  
#include "capture.h"  
// @0503
#include "jrtpsend.h"
#include "yuv422_to_I420.h" 
#include "encoder.h"
#include "x264.h"

//#define CLEAR(x) memset (&(x), 0, sizeof (x))  
  
//typedef enum {  
//    IO_METHOD_READ, IO_METHOD_MMAP, IO_METHOD_USERPTR,  
//} io_method;  
  
//struct buffer {  
//    void * start;  
//    size_t length;  
//};  
  
//char * dev_name = NULL; 
//int frame_num=0; 
//io_method io = IO_METHOD_MMAP;  
//fd = -1;  
//struct buffer * buffers = NULL;  
//unsigned int n_buffers = 0;  
  
//FILE *fp;  
//char *filename = "test.yuv\0";  
 io_method io = IO_METHOD_MMAP;  
 int fd = -1;  
 struct buffer * buffers= NULL;  
 unsigned int n_buffers = 0;  
 char * dev_name = "/dev/video0";
FILE *fp;


FILE *fpYUYV = NULL;
FILE *fpYUV420 = NULL;
FILE *fpH264 = NULL;

int bytesNALS = 0;



void openFiles()
{
    fpYUYV=fopen("testYUYV.yuv","wb");
    
    if( !fpYUYV )
    {
        printf("fpYUYV open failed!\n");
        return ;
    }

    fpYUV420 =fopen("testYUYV420.yuv","wb");
    
    if( !fpYUV420 )
    {
        printf("pYUV420  open failed!\n");
        return ;
    }

    fpH264 =fopen("testH264.h264","wb");
    
    if( !fpH264 )
    {
        printf("fpH264 open failed!\n");
        return ;
    }
}

 void errno_exit(const char * s) {  
    fprintf(stderr, "%s error %d, %s/n", s, errno, strerror(errno));  
  
    exit(EXIT_FAILURE);  
}  
  
 int xioctl(int fd, int request, void * arg) {  
    int r;  
  
    do {  
        r = ioctl(fd, request, arg);  
    } while (-1 == r && EINTR == errno);  
  
    return r;  
}  
  
 void process_image(const void * p, int size) {  
    // @0503 
    //fwrite(p, size, 1, fp); 
    // sws_scale(sws_ctx,(const uint8_t * const*)src_data,src_linesize,0,src_height,dst_data,dst_linesize);
    //sws_scale(sws_ctx,(const uint8_t * const*)p,src_linesize,0,src_height,dst_data,dst_linesize);
    


    // printf("before  422  2  420\n");
    // unsigned char buffI420p_2[640*480*3/2];
    // YUV422To420x264((unsigned char *)p, buffI420p_2, src_width,src_height);
    //  printf("after  422  2  420\n");
    // x264_nal_t  *nals;
    // x264_nal_t  *nal;
    // int nnal;
    // //yuv_buffer = buffI420p;
    // memcpy(yuv_buffer, buffI420p, yuv_size);
    // int bytesNALS = 0;
    // bytesNALS = x264_encoder_encode(encoder,&nals,&nnal,&pic_in,&pic_out);
    // printf("%d\n",bytesNALS );
    //  printf("before  data write \n");   
    // for(nal=nals;nal<nals+nnal;nal++)
    // {
    //     printf("**************%d\n",nnal );
    //     write(outf,nal->p_payload,nal->i_payload);
    // }

}  
  
 int read_frame(void) {  
    struct v4l2_buffer buf;  
    unsigned int i; 


    char tmpH264path[20] = "tmpH264.h264"; 
    FILE *tmpFP264 = fopen("tmpH264.h264","wr"); 
  
    switch (io) {  
    case IO_METHOD_READ:  
        if (-1 == read(fd, buffers[0].start, buffers[0].length)) {  
            switch (errno) {  
            case EAGAIN:  
                return 0;  
  
            case EIO:  
                /* Could ignore EIO, see spec. */  
                /* fall through */  
            default:  
                errno_exit("read");  
            }  
        }  
  
        process_image(buffers[0].start, buffers[0].length);  
  
        break;  
  
    case IO_METHOD_MMAP:  
        CLEAR(buf);  
  
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;  
        buf.memory = V4L2_MEMORY_MMAP;  
  
        if (-1 == xioctl(fd, VIDIOC_DQBUF, &buf)) {  
            switch (errno) {  
            case EAGAIN:  
                return 0;  
  
            case EIO:  
                /* Could ignore EIO, see spec. */  
  
                /* fall through */  
  
            default:  
                errno_exit("VIDIOC_DQBUF");  
            }  
        }  
  
        assert(buf.index < n_buffers);  
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
//--->->-YUYV---------------------------------------------------------------------------  
        /////////process_image(buffers[buf.index].start, buf.length); 
                // --->->-YUYV-----
                // fwrite(buffers[buf.index].start, 1, buf.length, fpYUYV); 
                // printf("writing fpYUYV data  into  the fpYUYV\n"); 
//-->->--YUV420----------------------------------------------------------------------------
        //////fread(src_data[0],1,src_bufsize,src_file)==src_bufsize);
     // @ FANAN--001   
        // printf("src_bufsize \t%d\n",src_bufsize);
        // memcpy(src_data[0],buffers[buf.index].start,src_bufsize);
        // sws_scale(sws_ctx,(const uint8_t * const*)src_data,src_linesize,0,src_height,dst_data,dst_linesize);
        // fwrite(dst_data[0],1,dst_bufsize,fpYUV420);
        // printf("writing YUYV420 data  into  the fpYUV420\n");  
     // @ FANAN--002 
        //printf("before  422  2  420\n");
        unsigned char buffI420p_2[640*480*3/2];
        YUV422To420x264((unsigned char *)buffers[buf.index].start, buffI420p_2, src_width,src_height);
                 // -->->--YUV420----
                 // if(fwrite(buffI420p_2,1,I420size,fpYUV420) <= 0)
                 // {
                 //    printf("writing YUYV420 data  ******* failed ****\n"); 
                 // }
         //printf("after  422  2  420\n"); 
//------------------------------------------------------------------------------------
        x264_nal_t  *nals;
        x264_nal_t  *nal;
        int nnal;
        //memcpy(yuv_buffer, buffI420p, yuv_size);

        // @ FANAN--001 
        //memcpy(yuv_buffer, dst_data, yuv_size);
        // @ FANAN--002 
        memcpy(yuv_buffer, buffI420p_2, yuv_size);        
        bytesNALS = x264_encoder_encode(encoder,&nals,&nnal,&pic_in,&pic_out);
        if (bytesNALS <= 0)
        {
            printf("bytesNALS = x264_encoder_encode() <= 0 *****  Failed \n");
            return 0;
        }
        printf(" x264_encoder_encode --->>----->>--  bytesNALS   %d\n",bytesNALS );
        
        //@ 0505 refuse using tempH264 file to transfer 
        // if(!tmpFP264)
        // {
        //     printf("error  on  fopen(\"tmpH264.h264\",\"wr\")\n");
        // }

        for(nal=nals;nal<nals+nnal;nal++)
        {
            printf("*****  nnal  *********%d\n",nnal );
            //fwrite(fpH264,nal->p_payload,nal->i_payload);
            if (!nal->p_payload)
            {
                printf("nal->p_payload    is NULL   \n");
            }


            SendH264Nalu(nal->p_payload,nal->i_payload);  
            RTPTime::Wait(RTPTime(1,0));   

            //@ 0505 refuse using tempH264 file to transfer
            // int fwriteNum;
            // fwriteNum = fwrite( nal->p_payload, 1, nal->i_payload, tmpFP264);
            // if(fwriteNum <= 0)
            // {
            //     printf("error  on  fwrite  x264\n");
            // }

        }

        // @0505 tempH264
        //fclose(tmpFP264);

        //------->-->--> --- write data into the file --->--->----

        // @0505 tempH264
        //rtpSend(tmpH264path);

//------------------------------------------------------------------------------------------------------
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
        if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))  
            errno_exit("VIDIOC_QBUF");  
  
        break;  
  
    case IO_METHOD_USERPTR:  
        CLEAR(buf);  
  
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;  
        buf.memory = V4L2_MEMORY_USERPTR;  
  
        if (-1 == xioctl(fd, VIDIOC_DQBUF, &buf)) {  
            switch (errno) {  
            case EAGAIN:  
                return 0;  
  
            case EIO:  
                /* Could ignore EIO, see spec. */  
  
                /* fall through */  
  
            default:  
                errno_exit("VIDIOC_DQBUF");  
            }  
        }  
  
        for (i = 0; i < n_buffers; ++i)  
            if (buf.m.userptr == (unsigned long) buffers[i].start  
                    && buf.length == buffers[i].length)  
                break;  
  
        assert(i < n_buffers);  
  
        process_image((void *) buf.m.userptr, buf.length);  
  
        if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))  
            errno_exit("VIDIOC_QBUF");  
  
        break;  
    }  
  
    return 1;  
}  
  
 void mainloop(int frame_num) {  
    unsigned int count;  
  
    count = frame_num;  
  
    while (count-- > 0) {  
        for (;;) {  
            fd_set fds;  
            struct timeval tv;  
            int r;  
  
            FD_ZERO(&fds);  
            FD_SET(fd, &fds);  
  
            /* Timeout. */  
            tv.tv_sec = 5;  
            tv.tv_usec = 0;  
  
            r = select(fd + 1, &fds, NULL, NULL, &tv);  
  
            if (-1 == r) {  
                if (EINTR == errno)  
                    continue;  
  
                errno_exit("select");  
            }  
  
            if (0 == r) {  
                fprintf(stderr, "select timeout/n");  
                exit(EXIT_FAILURE);  
            }  
  
            if (read_frame())  
                break;  
  
            /* EAGAIN - continue select loop. */  
        }  
    }  
}  
  
 void stop_capturing(void) {  
    enum v4l2_buf_type type;  
  
    switch (io) {  
    case IO_METHOD_READ:  
        /* Nothing to do. */  
        break;  
  
    case IO_METHOD_MMAP:  
    case IO_METHOD_USERPTR:  
        type = V4L2_BUF_TYPE_VIDEO_CAPTURE;  
  
        if (-1 == xioctl(fd, VIDIOC_STREAMOFF, &type))  
            errno_exit("VIDIOC_STREAMOFF");  
  
        break;  
    }  
}  
  
 void start_capturing(void) {  
    unsigned int i;  
    enum v4l2_buf_type type;  
  
    switch (io) {  
    case IO_METHOD_READ:  
        /* Nothing to do. */  
        break;  
  
    case IO_METHOD_MMAP:  
        for (i = 0; i < n_buffers; ++i) {  
            struct v4l2_buffer buf;  
  
            CLEAR(buf);  
  
            buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;  
            buf.memory = V4L2_MEMORY_MMAP;  
            buf.index = i;  
  
            if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))  
                errno_exit("VIDIOC_QBUF");  
        }  
  
        type = V4L2_BUF_TYPE_VIDEO_CAPTURE;  
  
        if (-1 == xioctl(fd, VIDIOC_STREAMON, &type))  
            errno_exit("VIDIOC_STREAMON");  
  
        break;  
  
    case IO_METHOD_USERPTR:  
        for (i = 0; i < n_buffers; ++i) {  
            struct v4l2_buffer buf;  
  
            CLEAR(buf);  
  
            buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;  
            buf.memory = V4L2_MEMORY_USERPTR;  
            buf.index = i;  
            buf.m.userptr = (unsigned long) buffers[i].start;  
            buf.length = buffers[i].length;  
  
            if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))  
                errno_exit("VIDIOC_QBUF");  
        }  
  
        type = V4L2_BUF_TYPE_VIDEO_CAPTURE;  
  
        if (-1 == xioctl(fd, VIDIOC_STREAMON, &type))  
            errno_exit("VIDIOC_STREAMON");  
  
        break;  
    }  
}  
  
 void uninit_device(void) {  
    unsigned int i;  
  
    switch (io) {  
    case IO_METHOD_READ:  
        free(buffers[0].start);  
        break;  
  
    case IO_METHOD_MMAP:  
        for (i = 0; i < n_buffers; ++i)  
            if (-1 == munmap(buffers[i].start, buffers[i].length))  
                errno_exit("munmap");  
        break;  
  
    case IO_METHOD_USERPTR:  
        for (i = 0; i < n_buffers; ++i)  
            free(buffers[i].start);  
        break;  
    }  
  
    free(buffers);  
}  
  
 void init_read(unsigned int buffer_size) {  
    buffers = (struct buffer *)calloc(1, sizeof(struct buffer ));  
  
    if (!buffers) {  
        fprintf(stderr, "Out of memory/n");  
        exit(EXIT_FAILURE);  
    }  
  
    buffers[0].length = buffer_size;  
    buffers[0].start = malloc(buffer_size);  
  
    if (!buffers[0].start) {  
        fprintf(stderr, "Out of memory/n");  
        exit(EXIT_FAILURE);  
    }  
}  
  
 void init_mmap(void) {  
    struct v4l2_requestbuffers req;  
  
    CLEAR(req);  
  
    req.count = 4;  
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;  
    req.memory = V4L2_MEMORY_MMAP;  
  
    if (-1 == xioctl(fd, VIDIOC_REQBUFS, &req)) {  
        if (EINVAL == errno) {  
            fprintf(stderr, "%s does not support "  
                    "memory mapping/n", dev_name);  
            exit(EXIT_FAILURE);  
        } else {  
            errno_exit("VIDIOC_REQBUFS");  
        }  
    }  
  
    if (req.count < 2) {  
        fprintf(stderr, "Insufficient buffer memory on %s/n", dev_name);  
        exit(EXIT_FAILURE);  
    }  
  
    buffers = (struct buffer *)calloc(req.count, sizeof(struct buffer ));  
  
    if (!buffers) {  
        fprintf(stderr, "Out of memory/n");  
        exit(EXIT_FAILURE);  
    }  
  
    for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {  
        struct v4l2_buffer buf;  
  
        CLEAR(buf);  
  
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;  
        buf.memory = V4L2_MEMORY_MMAP;  
        buf.index = n_buffers;  
  
        if (-1 == xioctl(fd, VIDIOC_QUERYBUF, &buf))  
            errno_exit("VIDIOC_QUERYBUF");  
  
        buffers[n_buffers].length = buf.length;  
        buffers[n_buffers].start = mmap(NULL /* start anywhere */, buf.length,  
                PROT_READ | PROT_WRITE /* required */,  
                MAP_SHARED /* recommended */, fd, buf.m.offset);  
  
        if (MAP_FAILED == buffers[n_buffers].start)  
            errno_exit("mmap");  
    }  
}  
  
 void init_userp(unsigned int buffer_size) {  
    struct v4l2_requestbuffers req;  
    unsigned int page_size;  
  
    page_size = getpagesize();  
    buffer_size = (buffer_size + page_size - 1) & ~(page_size - 1);  
  
    CLEAR(req);  
  
    req.count = 4;  
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;  
    req.memory = V4L2_MEMORY_USERPTR;  
  
    if (-1 == xioctl(fd, VIDIOC_REQBUFS, &req)) {  
        if (EINVAL == errno) {  
            fprintf(stderr, "%s does not support "  
                    "user pointer i/o/n", dev_name);  
            exit(EXIT_FAILURE);  
        } else {  
            errno_exit("VIDIOC_REQBUFS");  
        }  
    }  
  
    buffers = (struct buffer *)calloc(4, sizeof(struct buffer ));  
  
    if (!buffers) {  
        fprintf(stderr, "Out of memory/n");  
        exit(EXIT_FAILURE);  
    }  
  
    for (n_buffers = 0; n_buffers < 4; ++n_buffers) {  
        buffers[n_buffers].length = buffer_size;  
        buffers[n_buffers].start = memalign(/* boundary */page_size,  
                buffer_size);  
  
        if (!buffers[n_buffers].start) {  
            fprintf(stderr, "Out of memory/n");  
            exit(EXIT_FAILURE);  
        }  
    }  
}  
  
 void init_device(void) {  
    struct v4l2_capability cap;  
    struct v4l2_cropcap cropcap;  
    struct v4l2_crop crop;  
    struct v4l2_format fmt;  
    unsigned int min;  
  
    if (-1 == xioctl(fd, VIDIOC_QUERYCAP, &cap)) {  
        if (EINVAL == errno) {  
            fprintf(stderr, "%s is no V4L2 device/n", dev_name);  
            exit(EXIT_FAILURE);  
        } else {  
            errno_exit("VIDIOC_QUERYCAP");  
        }  
    }  
  
    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {  
        fprintf(stderr, "%s is no video capture device/n", dev_name);  
        exit(EXIT_FAILURE);  
    }  
  
    switch (io) {  
    case IO_METHOD_READ:  
        if (!(cap.capabilities & V4L2_CAP_READWRITE)) {  
            fprintf(stderr, "%s does not support read i/o/n", dev_name);  
            exit(EXIT_FAILURE);  
        }  
  
        break;  
  
    case IO_METHOD_MMAP:  
    case IO_METHOD_USERPTR:  
        if (!(cap.capabilities & V4L2_CAP_STREAMING)) {  
            fprintf(stderr, "%s does not support streaming i/o/n", dev_name);  
            exit(EXIT_FAILURE);  
        }  
  
        break;  
    }  
  
    /* Select video input, video standard and tune here. */  
  
    CLEAR(cropcap);  
  
    cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;  
  
    if (0 == xioctl(fd, VIDIOC_CROPCAP, &cropcap)) {  
        crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;  
        crop.c = cropcap.defrect; /* reset to default */  
  
        if (-1 == xioctl(fd, VIDIOC_S_CROP, &crop)) {  
            switch (errno) {  
            case EINVAL:  
                /* Cropping not supported. */  
                break;  
            default:  
                /* Errors ignored. */  
                break;  
            }  
        }  
    } else {  
        /* Errors ignored. */  
    }  
  
    CLEAR(fmt);  
  
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;  
    fmt.fmt.pix.width = 640;  
    fmt.fmt.pix.height = 480;  
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;  
    fmt.fmt.pix.field = V4L2_FIELD_TOP;  
  
    if (-1 == xioctl(fd, VIDIOC_S_FMT, &fmt))  
        errno_exit("VIDIOC_S_FMT");  
  
    /* Note VIDIOC_S_FMT may change width and height. */  
  
    /* Buggy driver paranoia. */  
    min = fmt.fmt.pix.width * 2;  
    if (fmt.fmt.pix.bytesperline < min)  
        fmt.fmt.pix.bytesperline = min;  
    min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;  
    if (fmt.fmt.pix.sizeimage < min)  
        fmt.fmt.pix.sizeimage = min;  
  
    switch (io) {  
    case IO_METHOD_READ:  
        init_read(fmt.fmt.pix.sizeimage);  
        break;  
  
    case IO_METHOD_MMAP:  
        init_mmap();  
        break;  
  
    case IO_METHOD_USERPTR:  
        init_userp(fmt.fmt.pix.sizeimage);  
        break;  
    }  
}  
  
 void close_device(void) {  
    if (-1 == close(fd))  
        errno_exit("close");  
  
    fd = -1;  
}  
  
 void open_device(void) {  
    struct stat st;  
  
    // if (-1 == stat(dev_name, &st)) {  
    //     fprintf(stderr, "Cannot identify '%s': %d, %s/n", dev_name, errno,  
    //             strerror(errno));  
    //     exit(EXIT_FAILURE);  
    // }  
  
    // if (!S_ISCHR(st.st_mode)) {  
    //     fprintf(stderr, "%s is no device/n", dev_name);  
    //     exit(EXIT_FAILURE);  
    // }  
  
    fd = open(dev_name, O_RDWR /* required */| O_NONBLOCK, 0);  
  
    if (-1 == fd) {  
        fprintf(stderr, "Cannot open '%s': %d, %s/n", dev_name, errno,  
                strerror(errno));  
        exit(EXIT_FAILURE);  
    }  
}  
  

