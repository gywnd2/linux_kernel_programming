#define _POSIX_C_SOURCE 199309L
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kthread.h>
#include <linux/spinlock.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/time.h>
#include <linux/random.h>

#define BILLION 1000000000

unsigned long long add_to_hp_list_count;

struct timespec64 spclock[2];
unsigned long long add_to_hp_list_time;

int counter;
struct mutex my_mutex;

struct task_struct *writer_thread1, *writer_thread2, *writer_thread3, *writer_thread4;

struct my_node{
    struct list_head entry;
    int data;
};

unsigned long long calclock3(struct timespec64 *spclock, unsigned long long *total_time, unsigned long long *total_count){
    long temp, temp_n;
    unsigned long long timedelay=0;
    if(spclock[1].tv_nsec>=spclock[0].tv_nsec){
        temp=spclock[1].tv_sec-spclock[0].tv_sec;
        temp_n=spclock[1].tv_nsec-spclock[0].tv_nsec;
        timedelay=BILLION*temp+temp_n;
    }else{
        temp=spclock[1].tv_sec-spclock[0].tv_sec-1;
        temp_n=BILLION+spclock[1].tv_nsec-spclock[0].tv_nsec;
        timedelay=BILLION*temp+temp_n;
    }

    __sync_fetch_and_add(total_time, timedelay);
    return timedelay;
}

static int writer_function(void *data){
    struct list_head my_list;

    /*initialize list*/
    INIT_LIST_HEAD(&my_list);
    
    struct my_node *current_node=NULL;
    struct my_node *tmp;

    unsigned int target;
    
    int i=0;
    int isFirst=1;
    // printk("!!!!Adding 25000 list elements!!!!");
    get_random_bytes(&target, sizeof(int));
    target%=25000;
    // printk("!!!! Delete random value %d from list!!!!", target);
    printk("!!!! Search random value %d from list!!!!", target);
    while(!kthread_should_stop()){
        // ktime_get_real_ts64(&spclock[0]);
        // mutex_lock(&my_mutex);
        // /*list element add*/
        // for(i=0; i<25000; i++){
        //     struct my_node *new=kmalloc(sizeof(struct my_node), GFP_KERNEL);
        //     new->data=i;
        //     list_add(&new->entry, &my_list);
        // }
        // mutex_unlock(&my_mutex);
        // if(isFirst){
        //     ktime_get_real_ts64(&spclock[1]);
        //     calclock3(spclock, &add_to_hp_list_time, &add_to_hp_list_count);
        //     printk("pid : %u, Mutex linked list insert time: %lluns", current->pid, add_to_hp_list_time);
        //     isFirst=0;
        // }
        // msleep(500);
        
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        // mutex_lock(&my_mutex);
        // /*list element add*/
        // for(i=0; i<25000; i++){
        //     struct my_node *new=kmalloc(sizeof(struct my_node), GFP_KERNEL);
        //     new->data=i;
        //     list_add(&new->entry, &my_list);
        // }

        // ktime_get_real_ts64(&spclock[0]);
        // // delete
        // list_for_each_entry_safe(current_node, tmp, &my_list, entry){
        //     if(current_node->data==target){
        //         list_del(&current_node->entry);
        //         kfree(current_node);
        //         break;
        //     }
        // }
        // mutex_unlock(&my_mutex);
        // if(isFirst){
        //     ktime_get_real_ts64(&spclock[1]);
        //     calclock3(spclock, &add_to_hp_list_time, &add_to_hp_list_count);
        //     printk("pid : %u, Mutex linked list delete time: %lluns", current->pid, add_to_hp_list_time);
        //     isFirst=0;
        // }
        // msleep(500);
        
        
    //     //////////////////////////////////////////////////////////////////////////////////////////////////////////////
            mutex_lock(&my_mutex);
            /*list element add*/
            for(i=0; i<25000; i++){
                struct my_node *new=kmalloc(sizeof(struct my_node), GFP_KERNEL);
                new->data=i;
                list_add(&new->entry, &my_list);
            }

            // search
            ktime_get_real_ts64(&spclock[0]);
            list_for_each_entry(current_node, &my_list, entry){
                if(current_node->data==target){
                        ktime_get_real_ts64(&spclock[1]);
                    break;
                }
            }
           mutex_unlock(&my_mutex);
        if(isFirst){
            ktime_get_real_ts64(&spclock[1]);
            calclock3(spclock, &add_to_hp_list_time, &add_to_hp_list_count);
            printk("pid : %u, Mutex linked list search time: %lluns", current->pid, add_to_hp_list_time);
            isFirst=0;
        }
            msleep(500);
    }
    do_exit(0);
}

int __init hello_module_init(void){
    printk("%s, Entering module \n", __func__);
    counter=0;
    mutex_init(&my_mutex);
    writer_thread1=kthread_run(writer_function, NULL, "writer_function");
    writer_thread2=kthread_run(writer_function, NULL, "writer_function");
    writer_thread3=kthread_run(writer_function, NULL, "writer_function");
    writer_thread4=kthread_run(writer_function, NULL, "writer_function");
    
    return 0;
}

void __exit hello_module_cleanup(void){
    kthread_stop(writer_thread1);
    kthread_stop(writer_thread2);
    kthread_stop(writer_thread3);
    kthread_stop(writer_thread4);
    printk("%s, Exiting module\n", __func__);
}

module_init(hello_module_init);
module_exit(hello_module_cleanup);

MODULE_LICENSE("GPL");