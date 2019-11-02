#include "List.h"

void List::insert(ListNode *t)
{
    switch(mode)
    {
    case FIFO:
        fifoinsert(t);
        break;
    case EDF:
        edfinsert(t);
        break;
    default:
        break;
    }
}
void List::sched()
{
    switch(mode)
    {
    case FIFO:
    {
        mexPrintf("acess FIFO\n");
        ListNode *tmp=head->succ;
        while(tmp!=tail)
        {
            mxArray *lh[1],*rh[1];
            rh[0]=mxCreateDoubleScalar((double)tmp->parameter);
            mexCallMATLAB(1,lh,1,rh,tmp->taskname);
            mexPrintf("callback succsess,%lf\n",mxGetScalar(lh[0]));
            tmp=tmp->succ;
            mxDestroyArray(lh[0]);
            mxDestroyArray(rh[0]);
        }
    }
    break;
    case FP:
    {

    }
    break;
    case EDF:
    {

    }
    break;
    default:
    break;
    }
}
void List::fifoinsert(ListNode *t)
{
    if(empty())
    {
        tail->pred=t;
        head->succ=t;
        t->succ=tail;
        t->pred=head;
        mexPrintf("insert done\n");
    }
    else
    {
        ListNode *tmp=head->succ;
        while(tmp!=tail)
        {
            if(tmp->starttime>t->starttime)
            {
                tmp->pred->succ=t;
                t->succ=tmp;
                t->pred=tmp->pred;
                tmp->pred=t;
                return;
            }
            tmp=tmp->succ;
        }
        tail->pred->succ=t;
        t->succ=tail;
        t->pred=tail->pred;
        tail->pred=t;
    }
    size+=1;
}
void List::edfinsert(ListNode *t)
{
    size+=1;
}
ListNode *List::front()
{
    return head->succ;
}
ListNode *List::back()
{
    return tail->pred;
}
void List::removeall()
{
    while(tail->pred!=head)
    {
        ListNode *t=tail->pred;
        t->succ->pred=t->pred;
        t->pred->succ=tail;
        delete t;
    }
    delete tail;
    delete head;
}