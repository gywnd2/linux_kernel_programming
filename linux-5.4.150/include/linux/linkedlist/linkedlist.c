#define _POSIX_C_SOURCE 199309L
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/time.h>
#include <linux/random.h>

#define BILLION 1000000000

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
    target%=100000;
    
    /*list element add*/
    printk("!!!!Adding 100000 list elements!!!!");
    int i=0;
    for(i=0; i<100000; i++){
        struct my_node *new=kmalloc(sizeof(struct my_node), GFP_KERNEL);
        new->data=i;
        list_add(&new->entry, &my_list);
    }
    
    /*list element delete*/
    printk("!!!!Deleting list element!!!!");
    ktime_get_real_ts64(&spclock[0]);

    list_for_each_entry_safe(current_node, tmp, &my_list, entry){
        if(current_node->data==target){
            list_del(&current_node->entry);
            ktime_get_real_ts64(&spclock[1]);
            printk("Deleted value : %d\n", current_node->data);
            kfree(current_node);
            __sync_fetch_and_add(&add_to_hp_list_count, 1);
            break;
        }
        __sync_fetch_and_add(&add_to_hp_list_count, 1);
    }

    calclock3(spclock, &add_to_hp_list_time, &add_to_hp_list_count);
    printk("time : %llu, count : %llu", add_to_hp_list_time, add_to_hp_list_count);

    // list_for_each_entry(current_node, &my_list, entry){
    //     if(current_node->data==target){
            
    //         printk("Found value : %d\n", current_node->data);
            
    //         break;
    //     }
        
    // }

    // /* check list*/
    // printk("!!!!Check list!!!!");
    // list_for_each_entry(current_node, &my_list, entry){
    //     printk("Current value : %d\n", current_node->data);
    // }

    /*check list reverse*/
    // printk("!!!!Check list reverse!!!!");
    // list_for_each_entry_reverse(current_node, &my_list, entry){
    //     printk("Current value : %d\n", current_node->data);
    // }

    // /*list element delete*/
    // printk("!!!!Deleting list elements!!!!");
    // struct my_node *tmp;
    // list_for_each_entry_safe(current_node, tmp, &my_list, entry){
    //     if(current_node->data==2){
    //         printk("Current node value : %d\n", current_node->data);
    //         list_del(&current_node->entry);
    //         kfree(current_node);
    //     }
    // }

    // /*check list*/
    // printk("!!!!Check list after all operations!!!!");
    // list_for_each_entry(current_node, &my_list, entry){
    //     printk("Current value : %d\n", current_node->data);
    // }

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