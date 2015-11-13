#ifndef JRTPSEND_H
#define JRTPSEND_H

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define PACKET_BUFFER_END (unsigned int)0x00000000
#define MAX_RTP_PKT_LENGTH 1360

#define H264 96

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <string.h>

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

extern RTPSession sess;
extern int status;

typedef struct
{
	unsigned char TYPE:5;
	unsigned char NRI:2;
	unsigned char F:1;

}NALU_HEADER;

typedef struct
{
	unsigned char TYPE:5;
	unsigned char NRI:2;
	unsigned char F:1;

}FU_INDICATOR;

typedef struct
{
	unsigned char TYPE:5;
	unsigned char R:1;
	unsigned char E:1;
	unsigned char S:1;

}FU_HEADER;

typedef struct
{
	int startcodeprefix_len;
	unsigned int len;
	unsigned int max_size;
	int forbidden_bit;
	int nal_reference_idc;
	int nal_unit_type;
	unsigned char *buf;
	unsigned short lost_packets;

}NALU_t;

extern FILE *bits;
extern NALU_HEADER *nalu_hdr;
extern FU_INDICATOR *fu_ind;
extern FU_HEADER *fu_hdr;
extern int info2,info3;
extern NALU_t *n;

	//RTPSession sess;
extern	int status;

void checkerror(int rtperr);
void CheckError(int rtperr);

void OpenBitstreamFile(char *fp);
NALU_t *AllocNALU(int buffersize);
int GetAnnexbNALU(NALU_t *nalu);
void dump(NALU_t *n);
int FindStartCode2(unsigned char *buf);
int FindStartCode3(unsigned char *buf);

void rtpSend(char *path);
void SendH264Nalu(unsigned char* m_h264Buf,int buflen); 






#endif
