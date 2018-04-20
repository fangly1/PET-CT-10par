
#include <math.h>
#include <stdlib.h>
#include "datastruct.h"

int CalcTime(PACKED_DATA *pData)
{   
    unsigned short MaxBin, Chan, Index;
    double t1,t2,t3,t4,t5,t6,t7,t8;
    double BaseTime, temp;
    if(!pData)
    {
        return -1;             
    }
    
    if((pData->ip < IP_MIN)||(pData->ip > IP_MAX))
    {        
        return -1;        
    }
    
    Chan = pData->RawData[3];    //get channel
    if((Chan < 1) || (Chan > 72))
    {
        return -1;         
    }
    pData->Chan = Chan;

    Index = (Chan - 1) / 24;
    MaxBin = pData->Maxbin[Index];
    BaseTime = (double)(pData->RawData[8 - 1]*(256*256*256*256.0)) + pData->RawData[7 - 1]*(256*256*256.0) + pData->RawData[6 - 1]*(256*256.0) + pData->RawData[5 - 1]*256.0;

    //cal t1
    temp = ( BaseTime + pData->RawData[10 - 1])*5.0;
    t1 = temp - (pData->RawData[9 - 1] - 128) * 5.0/MaxBin;

    //cal t2
    temp = (BaseTime + pData->RawData[14 - 1])*5.0;
    t2 = temp - (pData->RawData[13 - 1] - 128) * 5.0/MaxBin;
    if((int)(pData->RawData[14 - 1] - pData->RawData[10 - 1]) < -50)
    {
        t2 += 256*5;
    }
    else if((int)(pData->RawData[14 - 1] - pData->RawData[10 - 1]) > 50)
    {
        t2 -= 256*5;
    }

    //cal t3
    temp = (BaseTime + pData->RawData[18 - 1])*5.0;
    t3 = temp - (pData->RawData[17 - 1] - 128) * 5.0/MaxBin;
    if((int)(pData->RawData[18 - 1] - pData->RawData[10 - 1]) < -50)
    {
        t3 += 256*5;
    }
    else if((int)(pData->RawData[18 - 1] - pData->RawData[10 - 1]) > 50)
    {
        t3 -= 256*5;
    }

    //cal t4
    temp = (BaseTime + pData->RawData[22 - 1])*5.0;
    t4 = temp - (pData->RawData[21 - 1] - 128) * 5.0/MaxBin;
    if((int)(pData->RawData[22 - 1] - pData->RawData[10 - 1]) < -50)
    {
        t4 += 256*5;
    }
    else if((int)(pData->RawData[22 - 1] - pData->RawData[10 - 1]) > 50)
    {
        t4 -= 256*5;
    }

    //cal t5 
    temp = (BaseTime + pData->RawData[24 - 1])*5.0;
    t5 = temp - (64 - pData->RawData[23 - 1] + 128) * 5.0/MaxBin;
    if((int)(pData->RawData[24 - 1] < pData->RawData[10 - 1]))
    {
        t5 += 256*5;
    }

    //cal t6
    temp = (BaseTime + pData->RawData[20 - 1])*5.0;
    t6 = temp - (64 - pData->RawData[19 - 1] + 128) * 5.0/MaxBin;
    if((pData->RawData[20 - 1] < pData->RawData[10 - 1]))
    {
        t6 += 256*5;
    }

    //cal t7 
    temp = (BaseTime + pData->RawData[16 - 1])*5.0;
    t7 = temp - (64 - pData->RawData[15 - 1] + 128) * 5.0/MaxBin;
    if((pData->RawData[16 - 1] < pData->RawData[10 - 1]))
    {
        t7 += 256*5;
    }

    //cal t8
    temp = (BaseTime + pData->RawData[12 - 1])*5.0;
    t8 = temp - (64 - pData->RawData[11 - 1] + 128) * 5.0/MaxBin;
    if((pData->RawData[12 - 1] < pData->RawData[10 - 1]))
    {
        t8 += 256*5;
    }

    //time correction
    if(fabs(t1-t2) > 2)
    {
        t1 = t2 - 0.7;
    }
    if(t5 - t4 > 150)
    {
        t5 -= 256*5;
        t6 -= 256*5;
        t7 -= 256*5;
        t8 -= 256*5;
    }

    pData->EventTime[0] = 0;
    pData->EventTime[1] = t2-t1;
    pData->EventTime[2] = t3-t1;
    pData->EventTime[3] = t4-t1;
    pData->EventTime[4] = t5-t1;
    pData->EventTime[5] = t6-t1;
    pData->EventTime[6] = t7-t1;
    pData->EventTime[7] = t8-t1;

    // printf("\r\nip:%03d, Chan=%02d ", pData->ip, pData->Chan);
    // for(int n = 0; n < 8; n ++)
    // {
    //     printf(" %.4f", pData->EventTime[n]);
    // }
    // printf("\r\n");
    return 0;
}