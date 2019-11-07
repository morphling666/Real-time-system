function sched_init(args)

kernel_init(args)

data=1;
CreateTask(1,2,7,'test',-1,data)
CreateTask(2,3,6,'test1',-1,data)