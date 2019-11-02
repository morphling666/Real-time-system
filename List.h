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
    ListNode(char t[],int st,int dl,int para)
    {
        segment=1;
        strcpy(taskname,t);
        starttime=st;
        deadline=dl;
        parameter=para;
    }
    int segment;
    int starttime;
    int deadline;
    int parameter;
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
    }
    void insert(ListNode *t);
    ListNode *front();
    ListNode *back();
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
    void removeall();
    void fifoinsert(ListNode *t);
    void edfinsert(ListNode *t);
    ListNode *head;
    ListNode *tail;
};

List Taskchain;

#endif