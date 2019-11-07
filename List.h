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
    void segpop(mxArray **array);
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
    if(mxGetScalar(lh[0])!=-1)
    {
        Segptr[savesize]=lh[0];
        Segptr[savesize+1]=lh[1];
        Segptr[savesize+2]=mxCreateDoubleScalar(currentptr->taskid);
        savesize+=3;
    }
}
void List::segpop(mxArray **array)
{
    if((savesize==0)||(currentptr->taskid!=mxGetScalar(Segptr[savesize-1])))
    {
        mexPrintf("error lh rh\n");
        array[0]=mxCreateDoubleScalar(1);
        array[1]=mxDuplicateArray(currentptr->parameter);
        return;
    }
    array[1]=mxDuplicateArray(Segptr[savesize-2]);
    array[0]=mxDuplicateArray(Segptr[savesize-3]);
    savesize-=3;
    mexPrintf("savesize=%d\n",savesize);
}

int List::sched()
{
    if(timestamp==0)
        Segptr=new mxArray*[3*size];
    if((timestamp<currentptr->starttime)||(currentptr==tail))
    {
        timestamp++;
        return 0;
    }
    switch(mode)
    {
    case FIFO:
    {
        mexPrintf("acess FIFO,size=%d\n",size);
        ListNode *tmp=currentptr;
        mxArray *lh[2],*rh[2];
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
        ListNode *tmp=currentptr;
        mxArray *lh[2],*rh[2];
        segpop(rh);
        execute(lh,rh);
        tmp->nextsegment=mxGetScalar(lh[0]);
        int taskid=tmp->taskid;
        if(tmp->nextsegment==-1)
            currentptr=remove();
        return taskid;
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
        return 1;
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