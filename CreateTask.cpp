#include "List.cpp"

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    char str[100];
    if(nrhs!=2)
    {
        mexErrMsgTxt("parameter numbers error");
    }
    int x=mxGetScalar(prhs[0]);
    mxGetString(prhs[1], str, sizeof(str));
    ListNode *t=new ListNode(str,1,1,x);
    Taskchain.insert(t);
}
