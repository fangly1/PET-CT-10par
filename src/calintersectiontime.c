
#include <math.h>
#include <stdlib.h>
#include <float.h>
#include "calintersectiontime.h"

extern double g_Thr[];

//Find IntersectionPoint for each point
int FindIntersectionPoint(double pPar[7], double *pPoint)
{
    unsigned int StartPoint = 0;
    unsigned int EndPoint = 20;
    double Step = 0.02;
    double min = DBL_MAX;
    double Yr, Yf, t;
    unsigned int StepNum = (unsigned int)((EndPoint - StartPoint) / Step);
    double Ra, Rb, Fa, R, F, T0;
    unsigned int loop;

    T0 = pPar[1];
    F  = pPar[2];
    R  = pPar[3];
    Ra = pPar[4];
    Rb = pPar[5];
    Fa = pPar[6];
    t = StartPoint;
    for(loop = 0; loop < StepNum; loop ++)
    {
        Yr = Ra + Rb*(exp(R*(t - T0)));
        Yf = Fa*(exp(F*(t - T0)));
        if(fabs(Yr - Yf) < min)
        {
            min = fabs(Yr - Yf);
            *pPoint = t;
        }
        t += Step;
    }
    
    return 0;
}

//Cal Ra Rb Fa and find Intersection Point for each event
int CalIntersectionPoint(PACKED_DATA *pData)
{
    unsigned int loop;
    double Ra, Rb, Fa, X[EVENT_THR_NUM];
    double R;
    double F;
    double T0;
    double xAverage, yAverage;
    double tmp1, tmp2;
    double FunctionParam[7];
	double xTime = 0;
	
    xAverage = yAverage = 0;
    tmp1 = tmp2 = 0;
    FunctionParam[0] = pData->Param[0];
    T0 = FunctionParam[1] = pData->Param[1];
    F  = FunctionParam[2] = pData->Param[2];
    R  = FunctionParam[3] = pData->Param[3];

    for(loop = 0; loop < EVENT_THR_NUM/2; loop ++)
    {
        X[loop] = exp(R*(pData->EventTime[loop] - T0));
        xAverage += X[loop];
        yAverage += g_Thr[loop];
    }

    xAverage /= (EVENT_THR_NUM/2);
    yAverage /= (EVENT_THR_NUM/2);

    for(loop = 0; loop < EVENT_THR_NUM/2; loop ++)
    {
        tmp1 += (X[loop] - xAverage)*(g_Thr[loop] - yAverage);
        tmp2 += (X[loop] - xAverage)*(X[loop] - xAverage);
    }
    Rb = tmp1/tmp2;
    Ra = yAverage - Rb*xAverage;
    FunctionParam[4] = Ra;
    FunctionParam[5] = Rb;

    xAverage = yAverage = 0;
    tmp1 = tmp2 = 0;

    for(loop = EVENT_THR_NUM/2; loop < EVENT_THR_NUM; loop ++)
    {
        X[loop] = exp(F*(pData->EventTime[loop] - T0));
        tmp1 += X[loop] * g_Thr[loop];
        tmp2 += X[loop] * X[loop];
    }

    Fa = tmp1 / tmp2;
    FunctionParam[6] = Fa;

    FindIntersectionPoint(FunctionParam, &xTime);
	
	pData->IntersectionTime = xTime;

    return 0;
}