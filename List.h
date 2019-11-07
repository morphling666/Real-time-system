#ifndef LIST_H
#define LIST_H

#include <string.h>
#include <math.h>
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
        timestamp=0;
        TaskListptr=NULL;
        savesize=0;
        Segptr=NULL;
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
    int sched();
    ~List()
    {
        removeall();
    }
private:
    ListNode *remove();
    void removeall();
    void rePriority(ListNode *t);
    void segpop(mxArray *array[]);
    void execute(mxArray *lh[],mxArray *rh[]);
    int timestamp;
    ListNode *head;
    ListNode *tail;
    ListNode *currentptr;
    int savesize;
};

ListNode *List::remove()
{
    ListNode *c=currentptr;
    currentptr=currentptr->pred;
    currentptr->succ=c->succ;
    c->succ->pred=currentptr;
    c->pred=NULL;
    c->succ=NULL;
    delete c;
    size--;
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
    currentptr=head->succ;
    mexPrintf("insert done\n");
}
void List::execute(mxArray *lh[],mxArray *rh[])
{
    mexCallMATLAB(2,lh,2,rh,currentptr->taskname);
    mexPrintf("currenttaskid=%d\n",currentptr->taskid);
    for(int i=0;i<savesize;i++){
        mexPrintf("%lf\n",mxGetScalar(Segptr[i]));
    }
    mexPrintf("----split----\n");
    if(mxGetScalar(lh[0])!=-1)
    {
        Segptr[savesize]=lh[0];
        Segptr[savesize+1]=lh[1];
        Segptr[savesize+2]=mxCreateDoubleScalar(currentptr->taskid);
        mexMakeArrayPersistent(Segptr[savesize]);
        mexMakeArrayPersistent(Segptr[savesize+1]);
        mexMakeArrayPersistent(Segptr[savesize+2]);
        savesize+=3;
    }
    for(int i=0;i<savesize;i++){
        mexPrintf("%lf\n",mxGetScalar(Segptr[i]));
    }
}
void List::segpop(mxArray *array[])
{
    double id=(double)currentptr->taskid;
    if((savesize==0)||abs(id-mxGetScalar(Segptr[savesize-1]))>0.01)
    {
        array[0]=mxCreateDoubleScalar(1);
        array[1]=mxDuplicateArray(currentptr->parameter);
        return;
    }
    array[1]=mxDuplicateArray(Segptr[savesize-2]);
    array[0]=mxDuplicateArray(Segptr[savesize-3]);
    savesize-=3;
}

int List::sched()
{
    if((timestamp<currentptr->starttime)||(currentptr==tail)||(currentptr==head))
    {
        timestamp++;
        return 0;
    }
    mxArray *lh[2],*rh[2];
    switch(mode)
    {
    case FIFO:
    {
        mexPrintf("acess FIFO,size=%d\n",size);
        ListNode *tmp=currentptr;
        segpop(rh);
        mexPrintf("taskname=%s£¬para=%lf,%lf\n",tmp->taskname,mxGetScalar(rh[0]),mxGetScalar(rh[1]));
        execute(lh,rh);
        //return tmp->taskid;
        tmp->nextsegment=mxGetScalar(lh[0]);
        timestamp++;
        mexPrintf("callback succsess,%lf\n",mxGetScalar(lh[1]));
        int taskid=tmp->taskid;
        if(tmp->nextsegment==-1)
            currentptr=currentptr->succ;
        return taskid;
        //mxDestroyArray(lh[0]);
        //mxDestroyArray(rh[0]);
    }
    break;
    case FP:
    {
        mexPrintf("acess FP,size=%d\n",size);
        if(currentptr->succ!=tail&&timestamp>=currentptr->succ->starttime&&currentptr->priority>currentptr->succ->priority)
            currentptr=currentptr->succ;
        ListNode *tmp=currentptr;
        segpop(rh);
        mexPrintf("taskname=%s£¬para=%lf,%lf\n",tmp->taskname,mxGetScalar(rh[0]),mxGetScalar(rh[1]));
        execute(lh,rh);
        tmp->nextsegment=mxGetScalar(lh[0]);
        mexPrintf("seg=%d\n",tmp->nextsegment);
        timestamp++;
        int taskid=tmp->taskid;
        if(tmp->nextsegment==-1)
        {
            currentptr=remove();
            if(currentptr==head)
                currentptr=head->succ;
        }
        return taskid;
    }
    break;
    case RoundRobin:
    {
        mexPrintf("acess RoundRobin,size=%d\n",size);
        if(currentptr->succ!=tail&&timestamp>=currentptr->succ->starttime)
            currentptr=currentptr->succ;
        else if(currentptr->succ==tail)
        {
            currentptr=head->succ;
            if(currentptr==tail)
                return 0;
        }
        ListNode *tmp=currentptr;
        rh[0]=mxCreateDoubleScalar(tmp->nextsegment);
        rh[1]=mxDuplicateArray(tmp->parameter);
        mexCallMATLAB(2,lh,2,rh,currentptr->taskname);
        tmp->nextsegment=mxGetScalar(lh[0]);
        tmp->parameter=lh[1];
        mexMakeArrayPersistent(tmp->parameter);
        mexPrintf("seg=%d,data=%lf\n",tmp->nextsegment,mxGetScalar(tmp->parameter));
        timestamp++;
        int taskid=tmp->taskid;
        if(tmp->nextsegment==-1)
        {
            currentptr=remove();
            if(currentptr==head)
                currentptr=head->succ;
        }
        return taskid;
    }
    break;
    case EDF:
    {
        mexPrintf("acess EDF,size=%d\n",size);
        if(currentptr->succ!=tail&&timestamp>=currentptr->succ->starttime&&currentptr->priority>currentptr->succ->priority)
            currentptr=currentptr->succ;
        ListNode *tmp=currentptr;
        segpop(rh);
        execute(lh,rh);
        tmp->nextsegment=mxGetScalar(lh[0]);
        timestamp++;
        int taskid=tmp->taskid;
        if(tmp->nextsegment==-1)
        {
            currentptr=remove();
            if(currentptr==head)
                currentptr=head->succ;
        }
        return taskid;
    }
    break;
    default:
    {
        mexPrintf("Error Schedule Mode\n");
        return 0;
    }
    }
    return 0;
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