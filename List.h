#ifndef LIST_H
#define LIST_H

#include <string.h>
#include <mex.h>

#define FIFO 0
#define FP 1
#define RoundRobin 2
#define EDF 3

struct ListNode
{
    ListNode(){}
    ListNode(char t[],int st,int et,int dl,int pr,mxArray *para,int id)
    {
        nextsegment=1;
        exetime=et;
        priority=pr;
        memmove(taskname,t,100);
        starttime=st;
        deadline=dl;
        parameter=para;
        taskid=id;
    }
    int taskid;
    int nextsegment;
    int exetime;
    int priority;
    int starttime;
    int deadline;
    mxArray *parameter;
    char taskname[100];
    ListNode *pred;
    ListNode *succ;
};

class List
{
public:
    List()
    {
        head=new ListNode;
        tail=new ListNode;
        head->pred=nullptr;
        head->succ=tail;
        tail->succ=nullptr;
        tail->pred=head;
        size=0;
        timestamp=1;
        TaskListptr=NULL;
        Segptr=NULL;
        savesize=0;
    }
    void insert(ListNode *t);
    mxArray *TaskListptr;
    mxArray **Segptr;
    bool empty()
    {
        return head->succ==tail;
    }
    unsigned mode;
    unsigned size;
    void sched();
    ~List()
    {
        removeall();
    }
private:
    ListNode *remove();
    void removeall();
    void rePriority(ListNode *t);
    void segpop(mxArray **array);
    void execute(mxArray *lh[],mxArray *rh[],int count);
    int timestamp;
    ListNode *head;
    ListNode *tail;
    ListNode *currentptr;
    int savesize;
};

ListNode *List::remove()
{
    ListNode *c=currentptr;
    ListNode *t=currentptr->pred;
    t->succ=currentptr->succ;
    currentptr->succ->pred=t;
    c->pred=NULL;
    c->succ=NULL;
    delete c;
    currentptr=t;
    return currentptr;
}
void List::rePriority(ListNode *t)
{
    t->priority=t->deadline;
}
void List::insert(ListNode *t)
{
    if(mode==EDF)
        rePriority(t);
    if(empty())
    {
        tail->pred=t;
        head->succ=t;
        t->succ=tail;
        t->pred=head;
    }
    else
    {
        ListNode *tmp=head->succ;
        while(tmp!=tail)
        {
            if((tmp->starttime>t->starttime)||(tmp->starttime==t->starttime&&tmp->priority>t->priority))
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
    mexPrintf("insert done\n");
}
void List::execute(mxArray *lh[],mxArray *rh[],int count)
{
    while(count)
    {
        mexCallMATLAB(2,lh,2,rh,currentptr->taskname);
        rh[0]=lh[0];
        rh[0]=lh[1];
        count--;
    }
    if(currentptr->exetime>count)
    {
        Segptr[savesize]=rh[0];
        Segptr[savesize+1]=rh[1];
        savesize+=2;
    }
}
void List::segpop(mxArray **array)
{
    if(savesize==0)
    {
        array[0]=mxCreateDoubleScalar(1);
        array[1]=mxDuplicateArray(currentptr->parameter);
        return;
    }
    array[0]=mxDuplicateArray(Segptr[savesize-1]);
    array[1]=mxDuplicateArray(Segptr[savesize-2]);
    savesize-=2;
}

void List::sched()
{
    //Segptr=new mxArray*[2*size];
    switch(mode)
    {
    case FIFO:
    {
        mexPrintf("acess FIFO,size=%d\n",size);
        ListNode *tmp=head->succ;
        mxArray *lh[2],*rh[2];
        while(tmp!=tail)
        {
            lh[0]=mxCreateDoubleScalar(1);
            lh[1]=tmp->parameter;
            mexPrintf("taskname=%s£¬para=%lf\n",tmp->taskname,mxGetScalar(lh[1]));
            while(tmp->nextsegment!=-1)
            {
                rh[0]=lh[0];
                rh[1]=lh[1];
                mexCallMATLAB(2,lh,2,rh,tmp->taskname);
                mexPrintf("callback succsess,%lf\n",mxGetScalar(lh[1]));
                tmp->nextsegment=mxGetScalar(lh[0]);
            }
            tmp=tmp->succ;
            //mxDestroyArray(lh[0]);
            //mxDestroyArray(rh[0]);
        }
    }
    break;
    case FP:
    {
        mexPrintf("acess FP,size=%d\n",size);
        ListNode *tmp=head->succ;
        mxArray *lh[2],*rh[2];
        while(tmp!=tail&&tmp->succ!=tail)
        {
            currentptr=tmp;
            segpop(rh);
            if(tmp->priority<=tmp->succ->priority)
            {
                execute(lh,rh,tmp->exetime);
                timestamp=tmp->starttime+tmp->exetime;
                tmp=remove();
            }
            else
            {
                int gap=tmp->succ->starttime-timestamp;
                execute(lh,rh,gap);
                tmp=tmp->succ;
            }
            //mxDestroyArray(lh[0]);
            //mxDestroyArray(rh[0]);
        }
        currentptr=tmp;
        segpop(rh);
        execute(lh,rh,tmp->exetime);
    }
    break;
    case RoundRobin:
    {

    }
    break;
    case EDF:
    {

    }
    break;
    default:
    {
        mexPrintf("Error Schedule Mode\n");
        return;
    }
    }
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

List *Taskchain;

#endif