#include "List.h"
#include "getTaskchain.cpp"

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    Taskchain=getTaskchain();
    if(Taskchain==NULL)
        return;
    if(nrhs!=6)
    {
        mexErrMsgTxt("parameter numbers error");
    }
    char str[100];
    int starttime=mxGetScalar(prhs[0]);
    int exetime=mxGetScalar(prhs[1]);
    int deadline=mxGetScalar(prhs[2]);
    mxGetString(prhs[3],str,100);
    int priority=mxGetScalar(prhs[4]);
    mxArray *data=mxDuplicateArray(prhs[5]);
    mexMakeArrayPersistent(data);
    ListNode *t=new ListNode(str,starttime,exetime,deadline,priority,data,Taskchain->size+1);
    mexPrintf("id=%d\n",Taskchain->size+1);
    Taskchain->insert(t);
}
