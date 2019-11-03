function y=sched_init(arg)

kernel_init('FIFO')
y=1.0;
CreateTask(1.0,'test')