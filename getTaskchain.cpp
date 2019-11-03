#ifndef GETTASKCHAIN
#define GETTASKCHAIN

#include <mex.h>

List *getTaskchain()
{
  mxArray *var = (mxArray*)mexGetVariablePtr("global", "_TaskList");
  if (var == NULL) {
    mexPrintf("_TaskList not found!\nKernel primitives cannot be called from the command line!\n");
    return NULL;
  } 
  
  void *ptr = mxGetData(var);
  
  if (ptr != NULL) {
    List *r = (List *)*((void **)ptr);
    if (r == NULL) {
        mexPrintf("Taskchain is null - Fatal Error!!\n");
    }
    return r;
  } else  {
    mexPrintf("Pointer to Taskchain not found in _Taskchain!\n");
    return NULL;
  }
      
}

#endif

