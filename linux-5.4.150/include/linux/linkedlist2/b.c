#define _POSIX_C_SOURCE 199309L
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/time.h>
#include <linux/random.h>

#define BILLION 1000000000

//////////////////////////////////////////////////////////////////////////////////////////

d

//////////////////////////////////////////////////////////////////////////////////////////
struct timespec64 spclock[2];
unsigned long long add_to_hp_list_time;
unsigned long long add_to_hp_list_count;

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

void struct_example(void){
    struct list_head my_list;

    /*initialize list*/
    INIT_LIST_HEAD(&my_list);
    
    struct my_node *current_node=NULL;
    struct my_node *tmp;

    unsigned int target;
    get_random_bytes(&target, sizeof(int));
    target%=10000;
    
    /*list element add*/
    printk("!!!!Adding 10000 list elements with existing method!!!!");
    int i=0;
    ktime_get_real_ts64(&spclock[0]);
    for(i=0; i<10000; i++){
        struct my_node *new=kmalloc(sizeof(struct my_node), GFP_KERNEL);
        new->data=i;
        list_add(&new->entry, &my_list);
    }
    ktime_get_real_ts64(&spclock[1]);
    
    calclock3(spclock, &add_to_hp_list_time, &add_to_hp_list_count);
    printk("time : %llu", add_to_hp_list_time);

    /*list element add with vector*/
    printk("!!!!Adding 10000 list elements with vector method!!!!");
    ktime_get_real_ts64(&spclock[0]);
    for(i=0; i<10000; i++){
        vecPushBack(&v, i);
    }
    ktime_get_real_ts64(&spclock[1]);

    calclock3(spclock, &add_to_hp_list_time, &add_to_hp_list_count);
    printk("time : %llu", add_to_hp_list_time);

    /*check list*/
    printk("!!!!Traverse with existing method!!!!");
    ktime_get_real_ts64(&spclock[0]);
    list_for_each_entry(current_node, &my_list, entry){
    }
    ktime_get_real_ts64(&spclock[1]);

    calclock3(spclock, &add_to_hp_list_time, &add_to_hp_list_count);
    printk("time : %llu", add_to_hp_list_time);

    /*check list with vector*/
    printk("!!!!Traverse with vector method!!!!");
    ktime_get_real_ts64(&spclock[0]);
    i=0;
    for(i; (i<v.vecList.total-1); i++){
        v.vectorGet(&v, i);
    }
    ktime_get_real_ts64(&spclock[1]);
    
    calclock3(spclock, &add_to_hp_list_time, &add_to_hp_list_count);
    printk("time : %llu", add_to_hp_list_time);

    /*list element delete*/
    printk("!!!!Deleting list element with existing method!!!!");
    ktime_get_real_ts64(&spclock[0]);

    list_for_each_entry_safe(current_node, tmp, &my_list, entry){
        if(current_node->data==target){
            list_del(&current_node->entry);
            ktime_get_real_ts64(&spclock[1]);
            printk("Deleted value : %d\n", current_node->data);
            kfree(current_node);
            break;
        }
    }
    calclock3(spclock, &add_to_hp_list_time, &add_to_hp_list_count);
    printk("time : %llu", add_to_hp_list_time);

    /*list element delete with vector*/
    printk("!!!!Deleting list element with vector method!!!!");

    i=0;
    ktime_get_real_ts64(&spclock[0]);
    for(i; (i<v.vecList.total-1); i++){
        if(v.vecList.element==target){
            v.vectorDelete(&v, i);
        }
    }
    ktime_get_real_ts64(&spclock[1]);

    printk("Deleted value : %d\n", target);
    calclock3(spclock, &add_to_hp_list_time, &add_to_hp_list_count);
    printk("time : %llu", add_to_hp_list_time);

} 

int __init hello_module_init(void){
    struct_example();
    printk("Module init\n");
    return 0;
}

void __exit hello_module_cleanup(void){
    printk("Bye Module!\n");
}

module_init(hello_module_init);
module_exit(hello_module_cleanup);