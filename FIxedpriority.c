#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>
#include <linux/delay.h>

static struct task_struct *rt_threads[3];
static int rt_task_status[3] = {0, 0, 0}; // 0: stopped, 1: running
static int rt_task_prio[3] = {99, 89, 79};
static unsigned long rt_task_last_run[3] = {0, 0, 0};

int rt_thread_fn(void *data) {
    int id = *(int *)data;
    rt_task_status[id-1] = 1;
    while (!kthread_should_stop()) {
        rt_task_last_run[id-1] = jiffies;
        printk(KERN_INFO "[FixedSched] RT Task %d is running with priority %d at jiffies %lu\n", id, current->rt_priority, jiffies);
        ssleep(1);
    }
    rt_task_status[id-1] = 0;
    printk(KERN_INFO "[FixedSched] RT Task %d stopped\n", id);
    return 0;
}

// /proc interface for user-space frontend
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
static struct proc_dir_entry *proc_entry;

static int fixed_sched_proc_show(struct seq_file *m, void *v) {
    int i;
    seq_printf(m, "TaskID\tPriority\tStatus\tLastRun(jiffies)\n");
    for (i = 0; i < 3; i++) {
        seq_printf(m, "%d\t%d\t%s\t%lu\n", i+1, rt_task_prio[i], rt_task_status[i] ? "Running" : "Stopped", rt_task_last_run[i]);
    }
    return 0;
}
static int fixed_sched_proc_open(struct inode *inode, struct file *file) {
    return single_open(file, fixed_sched_proc_show, NULL);
}
static const struct proc_ops fixed_sched_proc_ops = {
    .proc_open = fixed_sched_proc_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};
static int __init fixed_sched_init(void) {
    int i;
    static int ids[3] = {1, 2, 3};
    struct sched_param param;

    for (i = 0; i < 3; i++) {
        rt_threads[i] = kthread_create(rt_thread_fn, &ids[i], "rt_task_%d", i);
        if (IS_ERR(rt_threads[i])) {
            printk(KERN_ERR "Failed to create RT task %d\n", i);
            continue;
        }

        param.sched_priority = rt_task_prio[i];
        sched_setscheduler(rt_threads[i], SCHED_FIFO, &param);
        wake_up_process(rt_threads[i]);
    }

    proc_entry = proc_create("fixed_sched", 0, NULL, &fixed_sched_proc_ops);
    if (!proc_entry) {
        printk(KERN_ERR "[FixedSched] Failed to create /proc/fixed_sched\n");
    }

    printk(KERN_INFO "[FixedSched] Fixed Priority Scheduling Module Loaded\n");
    return 0;
}

static void __exit fixed_sched_exit(void) {
    int i;
    for (i = 0; i < 3; i++) {
        if (rt_threads[i]) kthread_stop(rt_threads[i]);
    }
    if (proc_entry)
        proc_remove(proc_entry);
    printk(KERN_INFO "[FixedSched] Fixed Priority Scheduling Module Removed\n");
}

module_init(fixed_sched_init);
module_exit(fixed_sched_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("OS Expert");
MODULE_DESCRIPTION("Fixed Priority Scheduling via LKM");
