
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <sys/time.h>
#include <float.h>
#include "datastruct.h"
#include "caltime.h"
#include "FileList.h"
#include "callmcurve.h"
#include "calintersectiontime.h"

double   g_ParPerChan[IP_MAX][CHAN_MAX][4];
double   g_IntersectionPointPerChan[IP_MAX][CHAN_MAX];
unsigned int g_EventCounts[IP_MAX][CHAN_MAX];
unsigned short s_Maxbin[] = {55, 54, 55};
double g_Thr[8] = {30, 70, 110, 160, 160, 110, 70, 30};

double   g_ParPerChanTemp[IP_MAX][CHAN_MAX][4];
double   g_IntersectionPointPerChanTemp[IP_MAX][CHAN_MAX];
unsigned int g_EventCountsTemp[IP_MAX][CHAN_MAX];
unsigned int g_EventCountsTotalTemp = 0;

typedef struct _FORMULA_PARAM_{
	unsigned short ip;
	unsigned short Chan;
	float          param[8];
}FORMULA_PARAM;

PACKED_DATA *g_pPackData = NULL;

void PrintData(unsigned char *buf, int len)
{
    int loop;    
    if(!buf)
    {
        printf("[%s:%d]Invalid Pointer!\r\n", __FUNCTION__, __LINE__);   
        return ;     
    }
    
    for(loop = 0; loop < len; loop ++)
    {
        if(0 == loop % 16)
        {
            printf("\r\n");
        }
        printf(" %02X", buf[loop]);        
    }

    printf("\r\n");
}

//cal average of T0 F A
int CalParmAverage(PACKED_DATA *pData, unsigned int Count)
{
    unsigned int ipIndex, ChanIndex, ParmIndex, loop;

    for(loop = 0; loop < Count; loop ++)
    {
        if(pData->Maxbin[3])    //valid event
        {            
            g_EventCountsTemp[pData->ip - 1][pData->Chan - 1] ++;
            for(ParmIndex = 0; ParmIndex < 4; ParmIndex++)
            {
                g_ParPerChanTemp[pData->ip - 1][pData->Chan - 1][ParmIndex] += pData->Param[ParmIndex];
            }
        }
        pData ++;
    }
	
    for(ipIndex = IP_MIN; ipIndex <= IP_MAX; ipIndex ++)
    {
        for(ChanIndex = CHAN_MIN; ChanIndex <= CHAN_MAX; ChanIndex++)
        {            
            if(g_EventCountsTemp[ipIndex - 1][ChanIndex - 1])
            {                
                for(ParmIndex = 0; ParmIndex < 4; ParmIndex++)
                {
                    g_ParPerChanTemp[ipIndex - 1][ChanIndex - 1][ParmIndex] /= g_EventCountsTemp[ipIndex - 1][ChanIndex - 1];
                    g_ParPerChan[ipIndex - 1][ChanIndex - 1][ParmIndex] = ((g_ParPerChan[ipIndex - 1][ChanIndex - 1][ParmIndex] * g_EventCounts[ipIndex - 1][ChanIndex - 1])
                                                                    + (g_ParPerChanTemp[ipIndex - 1][ChanIndex - 1][ParmIndex] * g_EventCountsTemp[ipIndex - 1][ChanIndex - 1]))
                                                                    / (g_EventCounts[ipIndex - 1][ChanIndex - 1] + g_EventCountsTemp[ipIndex - 1][ChanIndex - 1]);
                }
                //printf("[%04d]IP:%03d, Chan=%02d Counts=%05d ", __LINE__, ipIndex, ChanIndex, g_EventCounts[ipIndex - 1][ChanIndex - 1]);
                // printf(" %.4f,%.4f,%.4f,%.4f\r\n", 
                // g_ParPerChan[ipIndex - 1][ChanIndex - 1][0],
                // g_ParPerChan[ipIndex - 1][ChanIndex - 1][1],
                // g_ParPerChan[ipIndex - 1][ChanIndex - 1][2],
                // g_ParPerChan[ipIndex - 1][ChanIndex - 1][3]);
            }
        }
    }

    // SaveT0BD2File();

    return 0;
}



//cal average of intersection time
int CalIntersectionTimeAverage(PACKED_DATA *pData, unsigned int Count)
{
    unsigned int ipIndex, ChanIndex, loop;

    for(loop = 0; loop < Count; loop ++)
    {
        if(pData->Maxbin[3])
        {            
            g_IntersectionPointPerChanTemp[pData->ip - 1][pData->Chan - 1] += pData->IntersectionTime;
        }
        pData ++;
    }
	
    for(ipIndex = IP_MIN; ipIndex <= IP_MAX; ipIndex ++)
    {
        for(ChanIndex = CHAN_MIN; ChanIndex <= CHAN_MAX; ChanIndex++)
        {            
            if(g_EventCountsTemp[ipIndex - 1][ChanIndex - 1])
            {
                g_IntersectionPointPerChanTemp[ipIndex - 1][ChanIndex - 1] /= g_EventCountsTemp[ipIndex - 1][ChanIndex - 1];
                g_IntersectionPointPerChan[ipIndex - 1][ChanIndex - 1] = ((g_IntersectionPointPerChan[ipIndex - 1][ChanIndex - 1] * g_EventCounts[ipIndex - 1][ChanIndex - 1])
                                                                + (g_IntersectionPointPerChanTemp[ipIndex - 1][ChanIndex - 1] * g_EventCountsTemp[ipIndex - 1][ChanIndex - 1]))
                                                                / (g_EventCounts[ipIndex - 1][ChanIndex - 1] + g_EventCountsTemp[ipIndex - 1][ChanIndex - 1]);                              
            }
            //printf("[%04d]IP:%03d, Chan=%02d Counts=%05d ", __LINE__, ipIndex, ChanIndex, g_EventCounts[ipIndex - 1][ChanIndex - 1]);
            //printf("Time=%.4f\r\n", g_IntersectionPointPerChan[ipIndex - 1][ChanIndex - 1]);
        }
    }
    return 0;
}

int UpdateEventCounts()
{
    unsigned int ipIndex, ChanIndex;

    for(ipIndex = IP_MIN; ipIndex <= IP_MAX; ipIndex ++)
    {
        for(ChanIndex = CHAN_MIN; ChanIndex <= CHAN_MAX; ChanIndex++)
        {        
            //if(g_EventCountsTemp[ipIndex - 1][ChanIndex - 1])
            {
                g_EventCounts[ipIndex - 1][ChanIndex - 1] += g_EventCountsTemp[ipIndex - 1][ChanIndex - 1];                            
            }
            //printf("[%04d]IP:%03d, Chan=%02d Counts=%05d ", __LINE__, ipIndex, ChanIndex, g_EventCounts[ipIndex - 1][ChanIndex - 1]);
        }
    }

    return 0;
}

int SaveParamData2File(void)
{
    unsigned int ipIndex, ChanIndex;
    unsigned char ipByte3, ipByte4;
	FORMULA_PARAM FPram;
    size_t st;
    FILE *fOut = NULL;

    fOut = fopen("lmparam.bin", "wb");
    if(!fOut)
    {
        printf("[%s:%d]Open file:%s err!\r\n", __FUNCTION__, __LINE__, "lmparam.bin");
        goto EXIT;        
    }
	memset((void*)(&FPram), 0, sizeof(FPram));
	
    for(ipIndex = IP_MIN; ipIndex <= IP_MAX; ipIndex ++)
    {
	    ipByte3 = 2;
        ipByte4 = (unsigned char)ipIndex;
		FPram.ip = (ipByte3 << 8)|ipByte4;        
        for(ChanIndex = CHAN_MIN; ChanIndex <= CHAN_MAX; ChanIndex++)
        {
	        //memset((void*)(&FPram), 0, sizeof(FPram));			
			FPram.Chan = (unsigned short)ChanIndex;
			FPram.param[3] = g_ParPerChan[ipIndex - 1][ChanIndex - 1][1];  //T0
			FPram.param[4] = g_ParPerChan[ipIndex - 1][ChanIndex - 1][2];  //F -- B0
			FPram.param[5] = g_ParPerChan[ipIndex - 1][ChanIndex - 1][3];  //R -- D0
			FPram.param[6] = g_IntersectionPointPerChan[ipIndex - 1][ChanIndex - 1];  //Intersection point time

			st = fwrite((void*)(&FPram), sizeof(FPram), 1, fOut);    
            if(st != 1)
            {
                printf("[%s:%d]Write Formula Param to file err!\r\n", __FUNCTION__, __LINE__);
                goto EXIT;  
            }             
        }
    }

EXIT:
    if(fOut)
    {
        fclose(fOut);
    }    
	return 0;
}

int SaveT0BD2File()
{
    FILE *fOut;
    unsigned int ipIndex, ChanIndex;
    double *pPram;
    size_t st;
    fOut = fopen("T0BD.bin", "wb");
    if(!fOut)
    {
        printf("open T0BD.bin err!\r\n");
        return -1;
    }

    for(ipIndex = IP_MIN; ipIndex <= IP_MAX; ipIndex ++)
    {
        for(ChanIndex = CHAN_MIN; ChanIndex <= CHAN_MAX; ChanIndex++)
        {      
            pPram = &(g_ParPerChan[ipIndex - 1][ChanIndex - 1][0]);   
            st = fwrite(pPram, sizeof(double), 4, fOut);
            if(st != 4)
            {
                printf("write ip:%d chan:%d err!\r\n", ipIndex, ChanIndex);
                fclose(fOut);
                return -1;                
            }
        }
    }  

    fclose(fOut);
    return 0;     
}

int SaveEventTimeandLmparam2File(PACKED_DATA *pData, unsigned int Count)
{
    FILE *fOut;
    unsigned int loop;
    double *pPram;
    size_t st;
    fOut = fopen("EventTimeandLmparam.bin", "wb");
    if(!fOut)
    {
        printf("open EventTime.bin err!\r\n");
        return -1;
    }

    for(loop = 0; loop < Count; loop ++)
    {
        pPram = (double *)(&(pData->EventTime[0]));
        st = fwrite(pPram, sizeof(double), 8, fOut);
        if(st != 8)
        {
            printf("write Eventtime err!\r\n");
            fclose(fOut);
            return -1;                
        }    
        pPram = (double *)(&(pData->Param[0]));
        st = fwrite(pPram, sizeof(double), 4, fOut);
        if(st != 4)
        {
            printf("write Eventlmparam err!\r\n");
            fclose(fOut);
            return -1;                
        }              
        pData++;
    }  

    fclose(fOut);
    return 0;  
}

int SaveEventTime2File(PACKED_DATA *pData, unsigned int Count)
{
    FILE *fOut;
    unsigned int loop;
    double *pPram;
    size_t st;
    fOut = fopen("EventTime.bin", "wb");
    if(!fOut)
    {
        printf("open EventTime.bin err!\r\n");
        return -1;
    }

    for(loop = 0; loop < Count; loop ++)
    {
        pPram = (double *)(&(pData->EventTime[0]));
        st = fwrite(pPram, sizeof(double), 8, fOut);
        if(st != 8)
        {
            printf("write Eventtime err!\r\n");
            fclose(fOut);
            return -1;                
        }        
        pData++;
    }  

    fclose(fOut);
    return 0;     
}

int SaveEventLmparam2File(PACKED_DATA *pData, unsigned int Count)
{
    FILE *fOut;
    unsigned int loop;
    double *pPram;
    size_t st;
    fOut = fopen("Eventlmparam.bin", "wb");
    if(!fOut)
    {
        printf("open Eventlmparam.bin err!\r\n");
        return -1;
    }

    for(loop = 0; loop < Count; loop ++)
    {
        pPram = (double *)(&(pData->Param[0]));
        st = fwrite(pPram, sizeof(double), 4, fOut);
        if(st != 4)
        {
            printf("write Eventlmparam err!\r\n");
            fclose(fOut);
            return -1;                
        }        
        pData++;
    }  

    fclose(fOut);
    return 0;     
}

int main(int argc, char* argv[])
{
    size_t st;
    int loop = 0, FileIndex;
    unsigned char PacketData[PER_PACKETDATA_LEN] = {0};    //
    unsigned int ip;
    unsigned int FileSize = 0;
    unsigned int EventCountsTotal = 0;
    unsigned int j;
    struct timeval StartTime, EndTime;
    FILE *fIn;
    PACKED_DATA  *pPackedData = NULL;
    char **pFileList;
    unsigned int FileCount = 0;
    char FilePath[512];

    // printf("sizeof(unsigned char)=%d\r\n", sizeof(unsigned char));
    // printf("sizeof(unsigned short)=%d\r\n", sizeof(unsigned short));
    // printf("sizeof(unsigned int)=%d\r\n", sizeof(unsigned int));
    // printf("sizeof(double)=%d\r\n", sizeof(double));

    if(argc < 2)
    {
        printf("[%s:%d]Cmdline Param err!\r\n", __FUNCTION__, __LINE__);
        return -1;
    }

    memset(g_ParPerChan, 0, sizeof(g_ParPerChan));
    memset(g_IntersectionPointPerChan, 0, sizeof(g_IntersectionPointPerChan));
    memset(g_EventCounts, 0, sizeof(g_EventCounts));

    memset(g_ParPerChanTemp, 0, sizeof(g_ParPerChanTemp));
    memset(g_IntersectionPointPerChanTemp, 0, sizeof(g_IntersectionPointPerChanTemp));
    memset(g_EventCountsTemp, 0, sizeof(g_EventCountsTemp));

    pFileList = GetFileNameList(argv[1], &FileCount);
    if(!pFileList)
    {
        printf("[%s:%d]Get file list:%s err!\r\n", __FUNCTION__, __LINE__, argv[1]);
        return -1;
    }

    for(FileIndex = 0; FileIndex < FileCount; FileIndex ++)
    {
        printf("[%02d]:%s\r\n", FileIndex, pFileList[FileIndex]);
    }

    for(FileIndex = 0; FileIndex < FileCount; FileIndex ++)
    {
        memset(FilePath, 0, sizeof(FilePath));
        snprintf(FilePath, sizeof(FilePath), "%s%s", argv[1], pFileList[FileIndex] );
        printf("Process File:%s\r\n", FilePath);
        fIn = fopen(FilePath, "rb");
        if(!fIn)
        {
            printf("[%s:%d]Open file:%s err!\r\n", __FUNCTION__, __LINE__, FilePath);
            goto EXIT;
        }
        else
        {
            printf("[%s:%d]Open file:%s OK!\r\n", __FUNCTION__, __LINE__, FilePath);
        }

        if(fseek(fIn, 0, SEEK_END) < -1)
        {
            printf("[%s:%d]fseek err!\r\n", __FUNCTION__, __LINE__);
            goto EXIT;
        }
        
        FileSize = ftell(fIn);

        if(fseek(fIn, FILEHEAD_LEN, SEEK_SET) < -1)
        {
            printf("[%s:%d]fseek err!\r\n", __FUNCTION__, __LINE__);
            goto EXIT;
        }

        EventCountsTotal = (FileSize - FILEHEAD_LEN) / PER_PACKETDATA_LEN * EVENTS_PER_PACKET;

        printf("FileSize=%d, EventCountsTotal=%d, Memory request:%luMB\r\n", FileSize, EventCountsTotal, 
                EventCountsTotal * sizeof(PACKED_DATA) / (1024 * 1024));
        g_pPackData = (PACKED_DATA*)malloc(EventCountsTotal * sizeof(PACKED_DATA));
        if(!g_pPackData)
        {
            printf("[%s:%d]malloc err!\r\n", __FUNCTION__, __LINE__);
            goto EXIT;            
        }  

        //load event data from file to buffer
        gettimeofday(&StartTime, NULL);
        pPackedData = g_pPackData;
        while(1)
        {
            memset(PacketData, 0, PER_PACKETDATA_LEN);
            st = fread((void*)PacketData, 1, PER_PACKETDATA_LEN, fIn);
            if(st < 0)
            {
                printf("[%s:%d]Read packet data err!\r\n", __FUNCTION__, __LINE__);
                goto EXIT;
            }  

            if(0 == st)
            {
                printf("[%s:%d]Read file EOF!\r\n", __FUNCTION__, __LINE__);
                break;
            }

            if(st < PER_PACKETDATA_LEN)
            {
                printf("[%s:%d]Read packet data-%Zu err!\r\n", __FUNCTION__, __LINE__, st);
                continue;
            }
                     
            ip = PacketData[IP_OFFSET_IN_PER_PACKET];
            //Load File to struct buffer
            for(loop=0; loop < EVENTS_PER_PACKET; loop ++)
            {
                pPackedData->ip = ip;
                for(j = 0; j < 3; j++)
                {
                    pPackedData->Maxbin[j] = s_Maxbin[j];
                }  
				#if 0
                for(j = 0; j < 8; j++)
                {
                    pPackedData->Thr[j] = g_Thr[j];
                }
				#endif
                memcpy(&(pPackedData->RawData[0]), PacketData + PER_EVENT_LEN * loop, PER_EVENT_LEN);
                pPackedData++;
            }          
        }
        fclose(fIn);
        fIn = NULL;
        gettimeofday(&EndTime, NULL);
        printf("[%d]Load file:%s Time elapse:%lds\r\n", __LINE__, FilePath, EndTime.tv_sec-StartTime.tv_sec);   

        pPackedData = g_pPackData;
        gettimeofday(&StartTime, NULL);
        //#pragma omp parallel for num_threads(8) schedule(static)
        #pragma omp parallel for
        for(loop=0; loop < EventCountsTotal; loop ++)
        {
            CalcTime((PACKED_DATA*)(pPackedData+loop));
            Callmcurve((PACKED_DATA*)(pPackedData+loop));       
        }  
        gettimeofday(&EndTime, NULL);
        printf("[%d]Cal time and lmcurve Time elapse:%lds\r\n", __LINE__, EndTime.tv_sec-StartTime.tv_sec);

        SaveEventTimeandLmparam2File(pPackedData, EventCountsTotal);
        //SaveEventTime2File(pPackedData, EventCountsTotal);
        //SaveEventLmparam2File(pPackedData, EventCountsTotal);

        //cal A T0 F R for per channel
        CalParmAverage(pPackedData, EventCountsTotal);

        gettimeofday(&StartTime, NULL);     
        //#pragma omp parallel for num_threads(8) schedule(static)
        #pragma omp parallel for
        for(loop=0; loop < EventCountsTotal; loop ++)
        {
            CalIntersectionPoint((PACKED_DATA*)(pPackedData+loop));
        }  
        gettimeofday(&EndTime, NULL);
        printf("[%d]Cal Intersection Point Time elapse:%lds\r\n", __LINE__, EndTime.tv_sec-StartTime.tv_sec);

        CalIntersectionTimeAverage(pPackedData, EventCountsTotal);

        UpdateEventCounts();
        memset(g_ParPerChanTemp, 0, sizeof(g_ParPerChanTemp));
        memset(g_IntersectionPointPerChanTemp, 0, sizeof(g_IntersectionPointPerChanTemp));
        memset(g_EventCountsTemp, 0, sizeof(g_EventCountsTemp));
        if(g_pPackData)
        {
            free(g_pPackData);
            g_pPackData = NULL;
        }        
    }

    SaveParamData2File();
	
EXIT:
    if(fIn)
    {
        fclose(fIn);
    }    

    if(g_pPackData)
    {
        free(g_pPackData);
    }
    
    return 0;
}

