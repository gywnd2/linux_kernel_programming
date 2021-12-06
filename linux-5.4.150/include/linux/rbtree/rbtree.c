#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/rbtree.h>
#include <linux/slab.h>
#include <linux/time.h>
#include <linux/random.h>

#define FALSE 0
#define TRUE 1
#define BILLION 1000000000

struct timespec64 spclock[2];
unsigned long long time;

unsigned long long calclock3(struct timespec64 *spclock, unsigned long long *total_time){
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

struct my_type{
    struct rb_node node;
    int key;
    int value;
};

int rb_insert(struct rb_root *root, struct my_type *data){
    struct rb_node **new=&(root->rb_node), *parent=NULL;

    /* Figure out "where" to put new node*/
    while(*new){
        struct my_type *this=container_of(*new, struct my_type, node);
        parent=*new;
        if(this->key>data->key)
            new=&((*new)->rb_left);
        else if(this->key<data->key)
            new=&((*new)->rb_right);
        else
            return FALSE;
    }

    rb_link_node(&data->node, parent, new);
    rb_insert_color(&data->node, root);

    return TRUE;
}

struct my_type *rb_search(struct rb_root *root, int key){
    struct rb_node *node = root->rb_node;
    while(node){
        struct my_type *data=container_of(node, struct my_type, node);
        if(data->key>key)
            node=node->rb_left;
        else if(data->key<key)
            node=node->rb_right;
        else
            return data;
    }
    return NULL;
}

int rb_delete(struct rb_root *mytree, int key){
    struct my_type *data=rb_search(mytree, key);
    if(data){
        rb_erase(&data->node, mytree);
        kfree(data);
        return TRUE;
    }
    return FALSE;
}

void struct_example(void){
    struct rb_root my_tree=RB_ROOT;
    int i=0, ret;

    unsigned int target;
    get_random_bytes(&target, sizeof(int));
    target%=100000;

    printk("!!!!Adding 100000 elements!!!!");

    /* rb_node create and insert */
    for(;i<100000;i++){
        struct my_type *new=kmalloc(sizeof(struct my_type), GFP_KERNEL);
        if(!new) return;
        new->value=i*10;
        new->key=i;
        ret=rb_insert(&my_tree, new);
    }

    /* rb_tree delete node */
    printk("!!!!Deleting node which key is : %d!!!!", target);
    ktime_get_real_ts64(&spclock[0]);
    rb_delete(&my_tree, target);
    ktime_get_real_ts64(&spclock[1]);
    
    calclock3(spclock, &time);
    printk("time : %llu", time);

    // /* rb_tree find node */
    // struct my_type *find_node=rb_search(&my_tree, target);
    // if(!find_node) return;
    // printk("find : (key, value)=(%d.%d)\n", find_node->key, find_node->value);

    

    // /* rb_tree traversal using itertor */
    // struct rb_node *node;
    // for(node=rb_first(&my_tree); node; node=rb_next(node))
    //     printk("(key, value)=(%d.%d)\n", rb_entry(node, struct my_type, node)->key, rb_entry(node, struct my_type, node)->value);
}
        
int __init hello_module_init(void){
    struct_example();
    printk("module init\n");
    return 0;
}

void __exit hello_module_cleanup(void){
    printk("Bye Module\n");
}

module_init(hello_module_init);
module_exit(hello_module_cleanup);