function sched_init(args)

kernel_init(args)

data=1;
mldata.res=50;
mldata.tau=0.005;
CreateTask(1,2,7,'test',1,data)
CreateTask(2,3,60,'test1',3,data)
CreateTask(4,52,56,'classifier',2,mldata)