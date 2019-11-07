#define S_FUNCTION_NAME Stest
#define S_FUNCTION_LEVEL 2

#include "simstruc.h"
#include "List.h"

static void mdlInitializeSizes(SimStruct *S)
{
    ssSetNumSFcnParams(S, 3);
    if (ssGetNumSFcnParams(S) != ssGetSFcnParamsCount(S)) {
        return; /* Parameter mismatch reported by the Simulink engine*/
    }
    mxArray *rhs[1],*lhs[1];
    const mxArray *arg;
    char initfunc[100];
    arg=ssGetSFcnParam(S,2);
    int m=(int)mxGetM(arg);
    int n=(int)mxGetN(arg);
    if(n!=2||m!=1)
    {
        mexPrintf("error port numbers,m=%d,n=%d\n",m,n);
        return;
    }
    Taskchain=new List;
    //mexMakeArrayPersistent(Taskchain->Segptr);
    if(mexGetVariablePtr("global","_TaskList")==0)
    {
        mxArray *var=mxCreateNumericMatrix(2,1,mxUINT64_CLASS,mxREAL);
        mexMakeArrayPersistent(var);
        mexPutVariable("global","_TaskList",var);
    }
    Taskchain->TaskListptr=(mxArray *)mexGetVariablePtr("global","_TaskList");
    if(Taskchain->TaskListptr==NULL)
    {
        mexPrintf("no _TaskList found.\n");
        return;
    }
    *((void **)mxGetData(Taskchain->TaskListptr))=(void *)Taskchain;
    mxGetString(ssGetSFcnParam(S, 0), initfunc, 100);
    rhs[0] = (mxArray *)ssGetSFcnParam(S, 1);
    if (mexCallMATLAB(0, NULL, 1,rhs, initfunc) != 0) {
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
    /* Take care when specifying exception free code - see sfuntmpl.doc */
    ssSetUserData(S,(void *)Taskchain);
    ssSetOptions(S,SS_OPTION_CALL_TERMINATE_ON_EXIT);
    Taskchain->sched();
    //mxDestroyArray(rhs[0]);
    //mxDestroyArray(lhs[0]);
}

static void mdlInitializeSampleTimes(SimStruct *S)
{
    ssSetSampleTime(S, 0, 1.0);//CONTINUOUS_SAMPLE_TIME
    ssSetOffsetTime(S, 0, 0.0);
}
static void mdlOutputs(SimStruct *S, int_T tid)
{
    Taskchain=(List *)ssGetUserData(S);
    mexPrintf("out\n");
    int_T i;
    InputRealPtrsType uPtrs = ssGetInputPortRealSignalPtrs(S,0);
    real_T *y1 = ssGetOutputPortRealSignal(S,0);
    real_T *y2 = ssGetOutputPortRealSignal(S,1);
    int_T width1 = ssGetOutputPortWidth(S,0);
    int_T width2 = ssGetOutputPortWidth(S,1);
    switch(Taskchain->mode)
    {
    case FIFO:
    {
        //Taskchain->sched();
        for (i=0; i<width1; i++) {
            *y1++ = 2.0 *(*uPtrs[i]);
        }
        for (i=0; i<width2; i++) {
            *y2++ =(Taskchain->size)*(*uPtrs[i]);
        }
    }
        break;
    default:
    {
    }
        break;
    }
}
static void mdlTerminate(SimStruct *S){ 
    Taskchain = (List *) ssGetUserData(S);
    //mxFree(Taskchain->Segptr);
    delete Taskchain;
    Taskchain = NULL;
    mxArray* rhs[2];
    rhs[0] = mxCreateString("global");
    rhs[1] = mxCreateString("_Taskchain");
    mexCallMATLAB(0, NULL, 2, rhs, "clear");
}
#ifdef MATLAB_MEX_FILE /* Is this file being compiled as a MEX-file? */
#include "simulink.c" /* MEX-file interface mechanism */
#else
#include "cg_sfun.h" /* Code generation registration function */
#endif
