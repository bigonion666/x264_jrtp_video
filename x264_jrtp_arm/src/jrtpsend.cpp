#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jrtpsend.h"

 FILE *bits=NULL;
 int FindStartCode2(unsigned char *buf);
 int FindStartCode3(unsigned char *buf);

 int info2=0,info3=0;

 NALU_HEADER *nalu_hdr;
 FU_INDICATOR *fu_ind;
 FU_HEADER *fu_hdr;

 void checkerror(int rtperr)
{
        if (rtperr < 0)
        {
        std::cout << "ERROR: " << RTPGetErrorString(rtperr) << std::endl;
                exit(-1);
        }
}

 void CheckError(int rtperr)
{
        if (rtperr < 0)
        {
        std::cout << "ERROR: " << RTPGetErrorString(rtperr) << std::endl;
                exit(-1);
        }
}


NALU_t *AllocNALU(int buffersize)
{

	NALU_t *n;
	if((n=(NALU_t *)calloc(1,sizeof(NALU_t)))==NULL)
	{
		printf("AllocNALU:n Wrong\n");
		exit(0);
	}

	n->max_size=buffersize;

	if((n->buf=(unsigned char *)calloc(buffersize,sizeof(char)))==NULL)
	{
		free(n);
		printf("AllocNALU n->buf Wrong\n");
		exit(0);
	}

	return n;
}


void FreeNALU(NALU_t *n)
{
	if(n)
	{
		if(n->buf)
		{
			free(n->buf);
			n->buf=NULL;
		}
		free(n);
	}

}


void OpenBitstreamFile(char *fp)
{
	if(NULL==(bits=fopen(fp,"rb")))
	{
		printf("open file error\n");
		exit(0);
	}
}


int GetAnnexbNALU(NALU_t *nalu)
{
	int pos=0;
	int StartCodeFound,rewind;  
	unsigned char *Buf;

	if((Buf=(unsigned char *)calloc(nalu->max_size,sizeof(char)))==NULL)
	{
		printf("GetAnnexbAlloc:Could not allocate Buf memory\n");
		exit(0);
	}
	//printf("GetAnnexbNAL  calloc  nalu->max_size:%d\n",nalu->max_size);
	nalu->startcodeprefix_len=3;

	if(3!=fread(Buf,1,3,bits))
	{
		printf("fread Buf 3bits is Wrong\n");
		free(Buf);
		return 0;
	}

	info2=FindStartCode2(Buf);
	if(info2!=1)
	{
			if(1!=fread(Buf+3,1,1,bits))
			{
				printf("fread Buf NO.4 is Wrong\n");
				free(Buf);
				return 0;
			}

			info3=FindStartCode3(Buf);
			if(info3!=1)
			{
				printf("There is no StartCode\n");
				free(Buf);
				return -1;
			}
														
			else
			{
				pos=4;
				nalu->startcodeprefix_len=4;
			}
	}
	else
	{		
		nalu->startcodeprefix_len=3;
		pos=3;
	}
	
		StartCodeFound=0;								
		info2=0;
		info3=0;

		while(!StartCodeFound)
		{
			if(feof(bits))
			{
				nalu->len  =  (pos-1)  -  nalu->startcodeprefix_len;
				memcpy(nalu->buf,&Buf[nalu->startcodeprefix_len],nalu->len);//no startcode
				nalu->forbidden_bit  =  nalu->buf[0]&0x80;
				nalu->nal_reference_idc  =  nalu->buf[0]&0x60;
				nalu->nal_unit_type  =  (nalu->buf[0])&0x1f;
				free(Buf);
				return pos-1;
			}

			Buf[pos++]=fgetc(bits);
			info3=FindStartCode3(&Buf[pos-4]);
			if(info3!=1)
			info2=FindStartCode2(&Buf[pos-3]);

			StartCodeFound=(info2==1||info3==1);
		}

		rewind=(info3==1)?-4:-3;

		if(0!=fseek(bits,rewind,SEEK_CUR))						
		{		
			free(Buf);								
			printf("GetAnnexNALU: Cannot fseek in the bit stream file\n");

		}	

		nalu->len  =  (pos+rewind) - nalu->startcodeprefix_len;
		memcpy(nalu->buf, &Buf[nalu->startcodeprefix_len], nalu->len);
		nalu->forbidden_bit  =  nalu->buf[0]&0x80;						
		nalu->nal_reference_idc  =  nalu->buf[0]&0x60;
		nalu->nal_unit_type  =  (nalu->buf[0])&0x1f;

		free(Buf);
		return (pos+rewind);

}


void dump(NALU_t *n)
{
	if(!n)
	{
		printf("dump NALU_t is Wrong\n");
		return;
	}

	//printf("n->len:%d\n",n->len);
	//printf("n->nal_unit_type:%x\n",n->nal_unit_type);

}


int FindStartCode2(unsigned char *Buf)
{
	if(Buf[0]!=0||Buf[1]!=0||Buf[2]!=1)
	{
		//printf("Cannot find start 0x000001\n");
				return 0;
	}
	else return 1;
}


int FindStartCode3(unsigned char *Buf)
{
	if(Buf[0]!=0||Buf[1]!=0||Buf[2]!=0||Buf[3]!=1)
	{
		return 0;	

	}
	else return 1;
}



void rtpSend(char *path)
{
	char sendbuf[1500];
	char *nalu_payload;
	int size=0;
	unsigned int timestamp_increase=0,ts_current=0;
	OpenBitstreamFile(path);
	NALU_t *n;

	n=AllocNALU(8000000);
	int start=false;
	while(!feof(bits))
	{	size=GetAnnexbNALU(n);   // huge bits usage 
		if(size<4)
		{
			printf("get nalu error!\n");
			continue;
		}
		//printf("GetAnnexbNALU(n) return size:%d\n",size);
		dump(n);
		if(!start)
		{
			if(n->nal_unit_type==1||n->nal_unit_type==5||n->nal_unit_type==6||n->nal_unit_type==7)
			{
				//printf("nal_unit_type== 1|5|6|7  jrtp sending begin\n");
				start=true;
			}	

		}	
		// send_PART_1
		if(n->len <= MAX_RTP_PKT_LENGTH)
		{
			nalu_hdr=(NALU_HEADER *)&sendbuf[0];
			nalu_hdr->F=n->forbidden_bit;
			nalu_hdr->NRI=n->nal_reference_idc>>5;
			nalu_hdr->TYPE=n->nal_unit_type;

			nalu_payload=&sendbuf[1];
			memcpy(nalu_payload,n->buf+1,n->len-1);
			ts_current=ts_current+timestamp_increase;

			if(n->nal_unit_type==1||n->nal_unit_type==5)
			{	
				//printf("line  SendPacket_A_1 \t260\n"); 
				status=sess.SendPacket((void *)sendbuf,n->len,96,true,3600);
			}

			else
			{
				//printf("line  SendPacket_A_2 \t266\n"); 
				status=sess.SendPacket((void *)sendbuf,n->len,96,true,0);
				continue;
			}
			checkerror(status);
		}
		// send_PART_2
		else if(n->len > MAX_RTP_PKT_LENGTH)
		{
			int k=0,l=0;
			//printf("%d\n",n->len );
			k=n->len/MAX_RTP_PKT_LENGTH;
			l=n->len%MAX_RTP_PKT_LENGTH;

			///______________________________
			if(l==0)
			{
				return ;
			}

			int t=0;

			while(t<=k)
			{
				if(!t)//first pkt package
				{
					memset(sendbuf,0,1500);
					fu_ind=(FU_INDICATOR *)&sendbuf[0];
					fu_ind->F=n->forbidden_bit;
					fu_ind->NRI=n->nal_reference_idc>>5;
					fu_ind->TYPE=28;//FU-A

					fu_hdr=(FU_HEADER *)&sendbuf[1];
					fu_hdr->E=0;
					fu_hdr->R=0;
					fu_hdr->S=1;
					fu_hdr->TYPE=n->nal_unit_type;

					nalu_payload = &sendbuf[2];
					memcpy(nalu_payload,n->buf+1,MAX_RTP_PKT_LENGTH);
					//printf("line  SendPacket_A_3 \t298\n"); 
					status=sess.SendPacket((void *)sendbuf,MAX_RTP_PKT_LENGTH+2,96,false,0);
					checkerror(status);
					t++;
				}

				else if(t==k)//last package
				{
					memset(sendbuf,0,1500);	
					fu_ind=(FU_INDICATOR *)&sendbuf[0];
					fu_ind->F=n->forbidden_bit;
		              fu_ind->NRI=n->nal_reference_idc>>5;
		              fu_ind->TYPE=28;//FU-A
					
					fu_hdr=(FU_HEADER *)&sendbuf[1];
					fu_hdr->R=0;
					fu_hdr->S=0;
					fu_hdr->E=1;
					fu_hdr->TYPE=n->nal_unit_type;

					nalu_payload = &sendbuf[2];
					memcpy(nalu_payload,n->buf+t*MAX_RTP_PKT_LENGTH+1,l-1);
					//printf("line  SendPacket_A_4 \t320\n"); 
					status=sess.SendPacket((void *)sendbuf,l+1,96,true,3600);			
					checkerror(status);

					t++;

				}

				else if((t<k)&&(t!=0))
				{
					memset(sendbuf,0,1500);
					fu_ind=(FU_INDICATOR *)&sendbuf[0];
		                        fu_ind->F=n->forbidden_bit;
		                        fu_ind->NRI=n->nal_reference_idc>>5;
		                        fu_ind->TYPE=28;//FU-A
					
					fu_hdr=(FU_HEADER *)&sendbuf[1];
		                        fu_hdr->R=0;
		                        fu_hdr->S=0;
		                        fu_hdr->E=0;//E=1
		                        fu_hdr->TYPE=n->nal_unit_type;

		             // printf("line  SendPacket_A_5 \t342\n"); 
					nalu_payload=&sendbuf[2];
					memcpy(nalu_payload,n->buf+t*MAX_RTP_PKT_LENGTH+1,MAX_RTP_PKT_LENGTH);
					status=sess.SendPacket((void *)sendbuf,MAX_RTP_PKT_LENGTH+2,96,false,0);
					checkerror(status);
					t++;
				}
			}		
		}
	}

	//  --->--->---> link to line 224 & 226
	//  memory bleak
	//  @0506
	FreeNALU(n);
	n = NULL;
	fclose(bits);


}
	
void SendH264Nalu(unsigned char* m_h264Buf,int buflen)
{
	unsigned char *pSendbuf; //发送数据指针  
     pSendbuf = m_h264Buf;  
      
    //去除前导码0x000001 或者0x00000001
    // It is  need to be for VLC  boardcast   
    if( 0x01 == m_h264Buf[2] )  
    {  
	     pSendbuf = &m_h264Buf[3];  
	     buflen -= 3;  
    }  
    else  
    {  
	     pSendbuf = &m_h264Buf[4];  
	     buflen -= 4;  
    }  
      
  
    char sendbuf[1430];   //发送的数据缓冲  
    memset(sendbuf,0,1430);  
  
    //int status;    
  
    printf("send packet length %d \n",buflen);  
  
    if ( buflen <= MAX_RTP_PKT_LENGTH )  
    {    
        memcpy(sendbuf,pSendbuf,buflen); 
        printf("line  SendPacket_1 \t389\n");   
        status = sess.SendPacket((void *)sendbuf,buflen);  
     
        CheckError(status);  
  
    }    
    else if(buflen > MAX_RTP_PKT_LENGTH)  
    {  
        //设置标志位Mark为0  
        sess.SetDefaultMark(false);  
        //printf("buflen = %d\n",buflen);  
        //得到该需要用多少长度为MAX_RTP_PKT_LENGTH字节的RTP包来发送  
        int k=0,l=0;    
        k = buflen / MAX_RTP_PKT_LENGTH;  
        l = buflen % MAX_RTP_PKT_LENGTH;  
        int t=0;//用指示当前发送的是第几个分片RTP包  
  
        char nalHeader = pSendbuf[0]; // NALU 头ª¡¤  
        while( t < k || ( t==k && l>0 ) )    
        {    
            if( (0 == t ) || ( t<k && 0!=t ) )//第一包到最后包的前一包  
            {  
                //  FANAN @ 001  	--------------->------------>-----
                sendbuf[0] = (nalHeader & 0x60)|28;   
                sendbuf[1] = (nalHeader & 0x1f); 
                if ( 0 == t ) 
                { 
                    sendbuf[1] |= 0x80; 
                } 
                printf("line  SendPacket_2 \t418\n"); 
                memcpy(sendbuf+2,&pSendbuf[t*MAX_RTP_PKT_LENGTH],MAX_RTP_PKT_LENGTH); 
                status = sess.SendPacket((void *)sendbuf,MAX_RTP_PKT_LENGTH+2);
                //  FANAN @ 002 		--------------->------------>-----
                // memcpy(sendbuf,&pSendbuf[t*MAX_RTP_PKT_LENGTH],MAX_RTP_PKT_LENGTH);  
                // status = SendPacket((void *)sendbuf,MAX_RTP_PKT_LENGTH);   
                //--------->-->---->----frame++ ------>----->---------
                CheckError(status); 
                t++; 
                //--------->-->---->----frame++ ------>----->--------- 
            }  
            //最后一包  
            else if( ( k==t && l>0 ) || ( t== (k-1) && l==0 ))  
            {  
                //设置标志位Mark为1  
                sess.SetDefaultMark(true);  
  
                int iSendLen;  
                if ( l > 0)  
                {  
                    iSendLen = buflen - t*MAX_RTP_PKT_LENGTH;  
                }  
                else  
                    iSendLen = MAX_RTP_PKT_LENGTH;  
  			//  FANAN @ 001  --------------->------------>-----
                sendbuf[0] = (nalHeader & 0x60)|28;    
                sendbuf[1] = (nalHeader & 0x1f);  
                sendbuf[1] |= 0x40; 
                printf("line  SendPacket_3\t446\n");  
                memcpy(sendbuf+2,&pSendbuf[t*MAX_RTP_PKT_LENGTH],iSendLen);  
                status = sess.SendPacket((void *)sendbuf,iSendLen+2);  

     		//  FANAN @ 002	--------------->------------>-----
                // memcpy(sendbuf,&pSendbuf[t*MAX_RTP_PKT_LENGTH],iSendLen);  
                // status = SendPacket((void *)sendbuf,iSendLen);  
  
                CheckError(status);  
                t++;  
            }  
        }  
    }  
}
