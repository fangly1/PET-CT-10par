
#ifndef ENERGY_DATASTRUCT_H
#define ENERGY_DATASTRUCT_H

#define  FILEHEAD_LEN  206
#define  PER_PACKETDATA_LEN  1154
#define  PER_EVENT_LEN 24
#define  CHAN_NUM   72
#define  CHAN_MIN   1
#define  CHAN_MAX   72
#define  IP_MIN   2
#define  IP_MAX   177
#define  IP_OFFSET_IN_PER_PACKET  1152
#define  EVENTS_PER_PACKET   48
#define  EVENT_THR_NUM     8
#define  EVENT_COUNTS_MAX  3000

#pragma pack(4)
typedef struct _PACK_DATA_
{
    unsigned short ip;    //ip
    unsigned short Chan;    //chanel
    unsigned short Maxbin[4];
    //float Thr[8];
    double Param[4];
    double EventTime[8];
	double IntersectionTime;
    unsigned char RawData[PER_EVENT_LEN];
}PACKED_DATA;
#pragma pack()

#endif