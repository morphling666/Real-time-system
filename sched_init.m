function y=sched_init(arg)

kernel_init('FIFO')

CreateTask(1,'test')
y=1;