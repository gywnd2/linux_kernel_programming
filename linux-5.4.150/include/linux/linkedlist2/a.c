#define _POSIX_C_SOURCE 199309L
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/time.h>
#include <linux/random.h>

#define BILLION 1000000000

//////////////////////////////////////////////////////////////////////////////////////////////

#define VECTOR_INIT_SIZE 10000
#define UNKNOWN -1
#define SUCCESS 0
#define VECTOR_INIT(vec) vector vec;\
vec_init(&vec);

typedef struct vectorList{
    void **element;
    int size;
    int total;
} vectorList;

typedef struct mVector vector;
struct mVector{
    vectorList vecList;

    int (*vectorTotal) (vector *);
    int (*vectorResize) (vector *, int);
    int (*vectorAdd) (vector *, void *);
    int (*vectorSet) (vector *, int, void *);
    void* (*vectorGet) (vector *, int);
    int (*vectorDelete) (vector *, int);
    int (*vectorFree) (vector *);
};

int vecTotal(vector *v){
    int total=UNKNOWN;
    if(v){
        total=v->vecList.total;
    }
    return total;
}

int vecResize(vector *v, int size){
    int status=UNKNOWN;
    if(v){
        void **element=krealloc(v->vecList.element, sizeof(void *) * size, GFP_KERNEL);
        if(element){
            v->vecList.element=element;
            v->vecList.size=size;
            status=SUCCESS;
        }
    }
    return status;
}

int vecPushBack(vector *v, void *item){
    int status=UNKNOWN;
    if(v){
        if(v->vecList.size==v->vecList.total){
            status=vecResize(v, v->vecList.size*2);
            if(status != UNKNOWN){
                v->vecList.element[v->vecList.total++]=item;
            }
            else{
                v->vecList.element[v->vecList.total++]=item;
                status=SUCCESS;
            }
        }
    }
    return status;
}

int vecSet(vector *v, int idx, void *item){
    int status=UNKNOWN;
    if(v){
        if((idx>=0)&&(idx<v->vecList.total)){
            v->vecList.element[idx]=item;
            status=SUCCESS;
        }
    }
    return SUCCESS;
}

void *vecGet(vector *v, int idx){
    void *readData=NULL;
    if(v){
        if((idx>=0)&&(idx<v->vecList.total)){
            readData=v->vecList.element[idx];
        }
    }
    return readData;
}

int vecDelete(vector *v, int idx){
    int status=UNKNOWN;
    int i=0;
    if(v){
        if((idx<0)||(idx>=v->vecList.total)){
            return status;
        }

        v->vecList.element[idx]=NULL;

        for(i=idx; (i<v->vecList.total-1); ++i){
            v->vecList.element[i]=v->vecList.element[i+1];
            v->vecList.element[i+1]=NULL;
        }

        v->vecList.total--;

        if((v->vecList.total>0)&&((v->vecList.total)==(v->vecList.size/4))){
            vecResize(v, v->vecList.size/2);
        }
        status=SUCCESS;
    }
    return status;
}

int vecFree(vector *v){
    int status=UNKNOWN;
    if(v){
        kfree(v->vecList.element);
        v->vecList.element=NULL;
        status=SUCCESS;
    }
    return status;
}

void vec_init(vector *v){
    v->vectorTotal=vecTotal;
    v->vectorResize=vecResize;
    v->vectorAdd=vecPushBack;
    v->vectorSet=vecSet;
    v->vectorGet=vecGet;
    v->vectorFree=vecFree;
    v->vectorDelete=vecDelete;
    v->vecList.size=VECTOR_INIT_SIZE;
    v->vecList.total=0;
    v->vecList.element=kmalloc(sizeof(void *) * v->vecList.size, GFP_KERNEL);
}

////////////////////////////////////////////////////////////////////////////////////////

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
    VECTOR_INIT(v);

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