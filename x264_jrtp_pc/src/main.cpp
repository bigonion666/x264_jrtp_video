#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <string.h>

#include "/usr/local/jthread/include/jthread/jmutex.h"

#include "jrtplib3/rtpsession.h"
#include "jrtplib3/rtppacket.h"
#include "jrtplib3/rtpudpv4transmitter.h"
#include "jrtplib3/rtpipv4address.h"
#include "jrtplib3/rtpsessionparams.h"
#include "jrtplib3/rtperrors.h"

#include "capture.h"
#include "encoder.h"
#include "yuv422_to_I420.h"
#include "jrtpsend.h"

#define true 1
#define false 0

//extern "C"{
//#ifdef __cplusplus
//#define __STDC_CONSTANT_MACROS
//#ifdef _STDINT_H
//#undef _STDINT_H
//#endif
//#include <stdint.h>
//#endif

//}

using namespace std;
//char * dev_name ;
int frame_num;
//FILE *fp=NULL;
const char *filename="test.yuv";
//int src_width=640;
//int src_height=480;
//enum AVPixelFormat src_pix_fmt=AV_PIX_FMT_YUYV422;
//enum AVPixelFormat dst_pix_fmt=AV_PIX_FMT_YUV420P;
//const char *dst_size="640x480";
//const char *dst_filename="frame_yuyv420.yuv";

//int x264_width=640;
//int x264_height=480;
//int fps=25;
size_t yuv_size_2=640*480*3/2;
//unsigned char *yuv_buffer=NULL;

typedef unsigned char uint8_t;

// void checkerror(int rtperr)
// {
//         if (rtperr < 0)
//         {
//         std::cout << "ERROR: " << RTPGetErrorString(rtperr) << std::endl;
//                 exit(-1);
//         }
// }

RTPSession sess;
int status;

int main(int argc, char ** argv)
{
        
        uint16_t portbase,destport;
        uint32_t destip;  
        std:string ipstr;  //get the destiny ip from the std input stream
        
     //   NALU_t *n;


        printf("Please Enter:./test   framenum(500)\n");
        if(argc!=2)
        {
                printf("Please Enter:./test   framenum\n");
                exit(EXIT_FAILURE);
        }

        //dev_name=argv[1];

        frame_num=atoi(argv[1]);    //convert a string to an integer
        if(!frame_num)
                {
                        printf("frame_num should be over 0 frame\n");
                        exit(EXIT_FAILURE);
                }

        // @0506 here ,there is no need to test YUYV | YUV420 | tmpH264 , for Module Test        
        //openFiles();

//*****************************************************************************        

//****************** the capturing process *****************start*********
                printf("capture video begin\n");
        open_device();
        init_device();
        start_capturing();
        //fp = fopen(filename, "wa+"); //yuv422

        // ***********
        //alloc_image();
        //  ***********

        // ***********
        //open_x264file("test420.yuv");
        init_encoder();
        init_picture(yuv_size_2);
        //  ***********
        portbase = 33334;
        destport = 9000;
        //ipstr = "222.197.174.76";
        ipstr = "202.115.11.128";
        destip = inet_addr(ipstr.c_str()); 
        //destip = ntohl(destip); ///************** very important **************
        destip = htonl(destip);

//********************** get the rtp parameter from std input stream ****above ***
        // @2 Setting basic parameter ----------------------------------
        RTPUDPv4TransmissionParams transparams;
        transparams.SetPortbase(portbase);


        RTPSessionParams sessparams;
        sessparams.SetOwnTimestampUnit(1.0/90000.0);

        status=sess.Create(sessparams,&transparams);
        checkerror(status);

        RTPIPv4Address addr(destip,destport);

        status = sess.AddDestination(addr);
        checkerror(status);

        sess.SetDefaultPayloadType(96);
        sess.SetDefaultMark(false);
        sess.SetDefaultTimestampIncrement(90000.0/10.0);  // frame rate control 10 / 25

        RTPTime delay(0.040);//RTPTime delay(0.040)
        //RTPTime::Wait(delay);
        RTPTime starttime=RTPTime::CurrentTime();

//         //  @3.0    Preparation for sending  NALU package----------------------------------------------------
//         char sendbuf[1500];
//         char *nalu_payload;
//         int size=0;
//         unsigned int timestamp_increase=0,ts_current=0;
//         OpenBitstreamFile("test.h264");

//         n=AllocNALU(8000000);// alloc memory for the <struct NALU>  n->maxSize = 8_000_000 = 8 MB
//         int start=false;

//         //****************************************************************

        mainloop(frame_num);   // **** capture frame *****
       // fclose(fp);
        stop_capturing();
        uninit_device();
        close_device();


        close_encoder();


                printf("capture video is over\n");
//****************** the capturing process ********************end ******


//****************** x264 encode ******************************* start ***
        // open_yuvfile(filename);
        // alloc_image();
        // swscale_start();     // ******  yuv422 -> I420 *******
        // swscale_close();

        // open_x264file(dst_filename);
        // init_encoder();
        // init_picture(yuv_size);
        // encode_frame();      // **** encode x264 format ***
        // close_encoder();
//****************** x264 encode ******************************* end ***

        /////---------------------------------------------------------------------------------------------------
                printf("encode x264 video is over\n");
                printf("Wait to jrtpsend....\n");


//******************  RTP send *********************** start **********
       // @1  Getting basic parameter -------------------------------
        // std::cout<<"Enter local portbase:"<<std::endl;
        // std::cin>>portbase;
        // std::cout<<std::endl;

        // std::cout << "Enter the destination IP address" << std::endl;
        // std::cin >> ipstr;
        // destip = inet_addr(ipstr.c_str());
        // if (destip == INADDR_NONE)
        // {
        //         std::cerr << "Bad IP address specified" << std::endl;
        //         return -1;
        // }
        // destip = ntohl(destip);

        // std::cout << "Enter the destination port" << std::endl;
        // std::cin >> destport;

//         portbase = 33333;
//         destport = 55555;
//         ipstr = "222.197.174.76";
//         destip = inet_addr(ipstr.c_str()); 

// //********************** get the rtp parameter from std input stream ****above ***
//         // @2 Setting basic parameter ----------------------------------
//         RTPUDPv4TransmissionParams transparams;
//         transparams.SetPortbase(portbase);


//         RTPSessionParams sessparams;
//         sessparams.SetOwnTimestampUnit(1.0/90000.0);

//         status=sess.Create(sessparams,&transparams);
//         checkerror(status);

//         RTPIPv4Address addr(destip,destport);

//         status = sess.AddDestination(addr);
//         checkerror(status);

//         sess.SetDefaultPayloadType(96);
//         sess.SetDefaultMark(false);
//         sess.SetDefaultTimestampIncrement(90000.0/10.0);

//         RTPTime delay(0.030);//RTPTime delay(0.040)
//         //RTPTime::Wait(delay);
//         RTPTime starttime=RTPTime::CurrentTime();

//         //  @3.0    Preparation for sending  NALU package----------------------------------------------------
//         char sendbuf[1500];
//         char *nalu_payload;
//         int size=0;
//         unsigned int timestamp_increase=0,ts_current=0;
//         OpenBitstreamFile("test.h264");

//         n=AllocNALU(8000000);// alloc memory for the <struct NALU>  n->maxSize = 8_000_000 = 8 MB
//         int start=false;
        //  NALU packages sending loop  ----------------------------------- 
        // while(!feof(bits))
        // {	
        //         size=GetAnnexbNALU(n);    // having original souce for this function
        //         if(size<4)
        //         {
        //                 printf("get nalu error!\n");
        //                 continue;
        //         }
        //         printf("size:%d\n",size);

        //         dump(n);
        //         if(!start)
        //         {
        //                  if(n->nal_unit_type==1||n->nal_unit_type==5||n->nal_unit_type==6||n->nal_unit_type==7)
        //                 {
        //                         printf("begin\n");
        //                         start=true;
        //                 }

        //         }

        //         if(n->len <= MAX_RTP_PKT_LENGTH)
        //         {
        //                 nalu_hdr=(NALU_HEADER *)&sendbuf[0];
        //                 nalu_hdr->F=n->forbidden_bit;
        //                 nalu_hdr->NRI=n->nal_reference_idc>>5;
        //                 nalu_hdr->TYPE=n->nal_unit_type;

        //                 nalu_payload=&sendbuf[1];
        //                 memcpy(nalu_payload,n->buf+1,n->len-1);
        //                 ts_current=ts_current+timestamp_increase;

        //                 if(n->nal_unit_type==1||n->nal_unit_type==5)
        //                 {
        //                         status=sess.SendPacket((void *)sendbuf,n->len,96,true,3600);
        //                 }

        //                 else
        //                 {
        //                         status=sess.SendPacket((void *)sendbuf,n->len,96,true,0);
        //                         continue;
        //                 }
        //                 checkerror(status);
        //         }

        //         else if(n->len > MAX_RTP_PKT_LENGTH)
        //         {
        //                 int k=0,l=0;
        //                 k=n->len/MAX_RTP_PKT_LENGTH;
        //                 l=n->len%MAX_RTP_PKT_LENGTH;
        //                 int t=0;

        //                 while(t<=k)
        //                 {
        //                         if(!t)//first pkt package
        //                         {
        //                                 memset(sendbuf,0,1500);
        //                                 fu_ind=(FU_INDICATOR *)&sendbuf[0];
        //                                 fu_ind->F=n->forbidden_bit;
        //                                 fu_ind->NRI=n->nal_reference_idc>>5;
        //                                 fu_ind->TYPE=28;//FU-A

        //                                 fu_hdr=(FU_HEADER *)&sendbuf[1];
        //                                 fu_hdr->E=0;
        //                                 fu_hdr->R=0;
        //                                 fu_hdr->S=1;
        //                                 fu_hdr->TYPE=n->nal_unit_type;

        //                                 nalu_payload=&sendbuf[2];
        //                                 memcpy(nalu_payload,n->buf+1,MAX_RTP_PKT_LENGTH);

        //                                 status=sess.SendPacket((void *)sendbuf,MAX_RTP_PKT_LENGTH+2,96,false,0);
        //                                 checkerror(status);
        //                                 t++;
        //                         }

        //                         else if(t==k)//last package
        //                         {
        //                                 memset(sendbuf,0,1500);
        //                                 fu_ind=(FU_INDICATOR *)&sendbuf[0];
        //                                 fu_ind->F=n->forbidden_bit;
        //                                 fu_ind->NRI=n->nal_reference_idc>>5;
        //                                 fu_ind->TYPE=28;//FU-A

        //                                 fu_hdr=(FU_HEADER *)&sendbuf[1];
        //                                 fu_hdr->R=0;
        //                                 fu_hdr->S=0;
        //                                 fu_hdr->E=1;
        //                                 fu_hdr->TYPE=n->nal_unit_type;

        //                                 nalu_payload=&sendbuf[2];
        //                                 memcpy(nalu_payload,n->buf+t*MAX_RTP_PKT_LENGTH+1,l-1);

        //                                 status=sess.SendPacket((void *)sendbuf,l+1,96,true,3600);
        //                                 checkerror(status);

        //                                 t++;

        //                         }

        //                         else if( (t<k) && (t!=0) ) //packageS  between the first and the last
        //                         {
        //                                 memset(sendbuf,0,1500);
        //                                 fu_ind=(FU_INDICATOR *)&sendbuf[0];
        //                                 fu_ind->F=n->forbidden_bit;
        //                                 fu_ind->NRI=n->nal_reference_idc>>5;
        //                                 fu_ind->TYPE=28;//FU-A

        //                                 fu_hdr=(FU_HEADER *)&sendbuf[1];
        //                                 fu_hdr->R=0;
        //                                 fu_hdr->S=0;
        //                                 fu_hdr->E=0;//E=1
        //                                 fu_hdr->TYPE=n->nal_unit_type;

        //                                 nalu_payload=&sendbuf[2];
        //                                 memcpy(nalu_payload,n->buf+t*MAX_RTP_PKT_LENGTH+1,MAX_RTP_PKT_LENGTH);
        //                                 status=sess.SendPacket((void *)sendbuf,MAX_RTP_PKT_LENGTH+2,96,false,0);
        //                                 checkerror(status);
        //                                 t++;
        //                         }
        //                 }

        //         }

        //         RTPTime::Wait(delay);//Wait(delay);
        //         RTPTime time=RTPTime::CurrentTime();
        //         time-=starttime;
        //         if(time>RTPTime(60.0))break;
        // }


        // //   @4   Finish   sending NALU package
        // printf("Mission over\n");
        // delay=RTPTime(10.0);
        // sess.BYEDestroy(delay,"Time's up",9);

        return 0;
}
