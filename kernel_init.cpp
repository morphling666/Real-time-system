#include "List.cpp"

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    char str[100];
    if(nrhs!=1)
    {
        mexErrMsgTxt("parameter numbers error");
    }
    mxGetString(prhs[0], str, sizeof(str));
    if(strcmp(str,"FIFO")==0)
        Taskchain.mode=FIFO;
    else if(strcmp(str,"FP")==0)
        Taskchain.mode=FP;
    else if(strcmp(str,"RoundRobin")==0)
        Taskchain.mode=RoundRobin;
    else if(strcmp(str,"EDF")==0)
        Taskchain.mode=EDF;
    else
        mexPrintf("error mode\n");
}