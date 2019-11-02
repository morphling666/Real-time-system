#define S_FUNCTION_NAME Stest
#define S_FUNCTION_LEVEL 2

#include "simstruc.h"
#include "List.cpp"
char initfunc[100];
static void mdlInitializeSizes(SimStruct *S)
{
    ssSetNumSFcnParams(S, 3);
    if (ssGetNumSFcnParams(S) != ssGetSFcnParamsCount(S)) {
        return; /* Parameter mismatch reported by the Simulink engine*/
    }
    mxArray *rhs[1],*lhs[1];
    const mxArray *arg;
    arg=ssGetSFcnParam(S,2);
    int m=(int)mxGetM(arg);
    int n=(int)mxGetN(arg);
    if(n!=2||m!=1)
    {
        mexPrintf("error port numbers,m=%d,n=%d\n",m,n);
        return;
    }
    mxGetString(ssGetSFcnParam(S, 0), initfunc, 100);
    strlen(initfunc);
    rhs[0] = (mxArray *)ssGetSFcnParam(S, 1);
    if (mexCallMATLAB(0, NULL, 1,rhs, "sched_init") != 0) {
        mexPrintf(" error\n");
        return;
    }
    ssSetNumInputPorts(S, 1);
    ssSetNumOutputPorts(S,2);
    ssSetInputPortDirectFeedThrough(S, 0, 1);
    ssSetInputPortWidth(S, 0, 1);
    ssSetOutputPortWidth(S, 0, 1);
    ssSetOutputPortWidth(S, 1, 1);
    ssSetNumContStates(S, 0);
    ssSetNumDiscStates(S, 0);
    ssSetNumSampleTimes(S, 1);
    ssSetNumRWork(S, 0);   /* real vector    */
    ssSetNumIWork(S, 0);   /* integer vector */
    ssSetNumPWork(S, 0);   /* pointer vector */
    ssSetNumModes(S, 0); 
    ssSetNumNonsampledZCs(S, 0);
    ssSetSimStateCompliance(S, USE_DEFAULT_SIM_STATE);
    /* Take care when specifying exception free code - see sfuntmpl.doc */
    ssSetOptions(S,  SS_OPTION_EXCEPTION_FREE_CODE);
    Taskchain.sched();
    mxDestroyArray(rhs[0]);
    mxDestroyArray(lhs[0]);
}

static void mdlInitializeSampleTimes(SimStruct *S)
{
    ssSetSampleTime(S, 0, CONTINUOUS_SAMPLE_TIME);
    ssSetOffsetTime(S, 0, 0.0);
}
static void mdlOutputs(SimStruct *S, int_T tid)
{
    int_T i;
    InputRealPtrsType uPtrs = ssGetInputPortRealSignalPtrs(S,0);
    real_T *y1 = ssGetOutputPortRealSignal(S,0);
    real_T *y2 = ssGetOutputPortRealSignal(S,1);
    int_T width1 = ssGetOutputPortWidth(S,0);
    int_T width2 = ssGetOutputPortWidth(S,1);
    for (i=0; i<width1; i++) {
        *y1++ = 2.0 *(*uPtrs[i]);
    }
    for (i=0; i<width2; i++) {
        if(!Taskchain.size)
            *y2++ = 4.0*(*uPtrs[i]);
        else
            *y2++ = 3.0 *(*uPtrs[i]);
    }
}
static void mdlTerminate(SimStruct *S){}

#ifdef MATLAB_MEX_FILE /* Is this file being compiled as a MEX-file? */
#include "simulink.c" /* MEX-file interface mechanism */
#else
#include "cg_sfun.h" /* Code generation registration function */
#endif
