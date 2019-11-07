function sched_init(args)

kernel_init(args)

data=1;
CreateTask(1,3,5,'test',-1,data)
CreateTask(2,2,6,'test1',-1,data)