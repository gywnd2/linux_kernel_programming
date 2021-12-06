#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include "mylist.h"
#include <linux/slab.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>

struct my_node {
        struct list_head list;
        int data;
};


unsigned long long calclock3(unsigned long long interval, unsigned long long *total_time, unsigned long long *total_count){
        //long temp, temp_n = 0;
        unsigned long long timedelay = interval;

        __sync_fetch_and_add(total_time, timedelay);
        __sync_fetch_and_add(total_count, 1);
        return timedelay;
}

void struct_example(void)
{
        struct list_head my_list;
        INIT_LIST_HEAD(&my_list);
        int i;
        for(i=1; i<101; i*=10){
                int k = 1000 * i;
                int value;
                unsigned long long count = 0;
                struct __kernel_timex spclock[2];
                unsigned long long list_time;
                unsigned long long list_count;
                unsigned long long starttime;
                unsigned long long endtime;
                //do_clock_adjtime(CLOCK_REALTIME, &spclock[0]);
                starttime = ktime_get_ns();
                for(value = 0; value < k; value++){
                        struct my_node *new = kmalloc(sizeof(struct my_node), GFP_KERNEL);
                        new->data = value;
                        list_add(&new->list, &my_list);
                        count+=1;
                }
                endtime = ktime_get_ns();
                printk("insert %d times, time: %llu, count: %llu \n", k, endtime-starttime, count);
                
                struct my_node *current_node;
                struct list_head *p;
                list_for_each(p, &my_list){
                        current_node = list_entry(p, struct my_node, list);
                }
                //do_clock_adjtime(CLOCK_REALTIME, &spclock[0]);
                count = 0;
                starttime = ktime_get_ns();
                list_for_each_entry(current_node, &my_list, list){
                        int cur = current_node->data;
                        count++;
                }
                //do_clock_adjtime(CLOCK_REALTIME, &spclock[1]);
                endtime = ktime_get_ns();
                //calclock3(endtime-starttime, &list_time, &list_count);
                printk("search %d times, time: %llu, count: %llu \n", k, endtime-starttime, count);

                struct my_node *tmp;
                count = 0;
                starttime = ktime_get_ns();
                list_for_each_entry_safe(current_node, tmp, &my_list, list){
                        list_del(&current_node->list);
                        kfree(current_node);
                        count++;
                }
                //do_clock_adjtime(CLOCK_REALTIME, &spclock[1]);
                endtime = ktime_get_ns();
                //calclock3(endtime-starttime, &list_time, &list_count);
                printk("delete %d times, time: %llu, count: %llu \n", k, endtime-starttime, count);
        }
}

int __init my_module_init(void)
{
        
        printk("start my new list! \n");
        struct_example();
        return 0;
}

void __exit my_module_cleanup(void)
{
        printk("Bye my new list!\n");
}

module_init(my_module_init);
module_exit(my_module_cleanup);
MODULE_LICENSE("GPL");