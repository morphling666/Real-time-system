#include "List.h"
#include "getTaskchain.cpp"

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    Taskchain=getTaskchain();
    if(Taskchain==NULL)
        return;
    if(nrhs!=2)
    {
        mexErrMsgTxt("parameter numbers error");
    }
    char str[100];
    int x=mxGetScalar(prhs[0]);
    mxGetString(prhs[1],str,100);
    ListNode *t=new ListNode(str,1,1,x);
    Taskchain->insert(t);
}
