
#include <math.h>
#include <stdlib.h>
#include "lmcurve.h"
#include "callmcurve.h"

extern double g_Thr[];

double f(const double t, const double *p )
{
    return (p[0] * exp((t - p[1])*p[2]) * (1 - exp((t - p[1])*p[3])));
}

int Callmcurve(PACKED_DATA *pData)
{
    int n = 4;
    int m = 8;
    double par[4] = {400, 0 , -0.025, -0.1};  //-1~0, -0.025~-0.045, -0.1~-0.2
    double time[8];
    double thr[8];
    double st[4] = {500, -0.6, -0.025,  -0.1};
    double LB[4] = {300, -100, -1/3.0,  -10};
    double UB[4] = {1500, 10, -0.025, -0.1};

    lm_control_struct control = lm_control_double;

    lm_status_struct status;
    control.verbosity = 0;    
    
    if((pData->ip < IP_MIN)||(pData->ip > IP_MAX))
    {
        return -1;        
    }

    if((pData->Chan < CHAN_MIN)||(pData->Chan > CHAN_MAX))
    {
        return -1;          
    }

    for(unsigned short k = 0; k < 8; k++)
    {
        //thr[k] = pData->Thr[k];
        thr[k] = g_Thr[k];
        time[k] = pData->EventTime[k];
    }
    
    lmcurve(n, par, m, time, thr, f, &control, &status);

    for(int k = 0; k < 4; k ++)
    {
        if((par[k] < LB[k]) || (par[k] > UB[k]))
        {
            par[k] = st[k];
        }
    }
    
    // if((par[1] < -1) || (par[1] > 0))
    // {
    //     return -1;
    // }

    // if((par[2] < -0.045) || (par[2] > -0.025))
    // {
    //     return -1;
    // }

    // if((par[3] < -0.2) || (par[3] > -0.1))
    // {
    //     return -1;
    // }

    for(int loop = 0; loop < 4; loop ++)
    {
        pData->Param[loop] = par[loop];
    }

    pData->Maxbin[3] ++; 
        
    return 0;
}