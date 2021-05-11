#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "spinlock.h"
#include "slab.h"
#include "stdbool.h"

struct {
	struct spinlock lock;
	struct slab slab[NSLAB];
} stable;

void slabinit(){
	int size = 8;

	initlock(&stable.lock, "stable");
	acquire(&stable.lock);
	for(struct slab *sl = stable.slab; sl < &stable.slab[NSLAB]; sl ++){
		sl->size = size;
		sl->num_pages= 1;
		sl->num_free_objects = 4096 / size;
		sl->num_used_objects = 0;
		sl->num_objects_per_page = 4096 / size;
		sl->bitmap = kalloc();
		memset(sl->bitmap, 0, 4096);
		sl->page[0] = kalloc();
		size =  size << 1;
	}
	release(&stable.lock);
}

bool get_bit(char *num, int i){
	return ((*num & (1 << i)) != 0);
}

char *set_bit(char *num, int i){
	return *num | (i << i);
}

char *clear_bit(char *num, int i){
	char mask = ~(1 << i);
	return *num & mask;
}



char *kmalloc(int size){
	if(size > 2048) return 0;
	struct slab *sl;
	
	int count = 0;
	int alloc_size;
	char *address = 0;
    if(size && !(size & (size-1)))
        alloc_size = size;

    while(size != 0){
        size >>= 1;
        count++;
    }
	alloc_size = 1 << count;

	acquire(&stable.lock);
	for(sl = stable.slab; sl < &stable.slab[NSLAB]; sl ++){
		if(sl->size == alloc_size){
			if(sl->num_free_objects > 0){
				if(sl->num_used_objects < sl->num_objects_per_page){
					for(int i = 0; i < sl->num_objects_per_page; i++){
						if(!get_bit(sl->bitmap, i)){
							sl->bitmap = set_bit(sl->bitmap, i);
							address = sl->page[sl->num_pages-1] + i * sl->size;
							sl->num_used_objects++;
							sl->num_free_objects--;
							break;
						}
					}
				}
			}
			else{
				sl->num_free_objects = sl->num_objects_per_page;
				sl->num_pages++;
				sl->page[sl->num_pages-1] = kalloc();
			}
		}
	}
	release(&stable.lock);
	return address;
}

void kmfree(char *addr, int size){
	int count = 0;
	int alloc_size;
    if(size && !(size & (size-1)))
        alloc_size = size;

    while(size != 0){
        size >>= 1;
        count++;
    }
	alloc_size = 1 << count;
	acquire(&stable.lock);
	for(struct slab *sl = stable.slab; sl < &stable.slab[NSLAB]; sl++){
		for(int i = 0; i < sl->num_pages; i++){
			for(int j = 0; j < sl->num_objects_per_page; j++){
				if(addr == sl->page[i] + (sl->size * j)){	
					if(sl->size == alloc_size){
						sl->num_free_objects++;
						sl->num_used_objects--;
					}
					clear_bit(sl->bitmap, i *sl->num_pages * j);
					if(sl->num_used_objects < (sl->num_pages - 1 + count) * sl->num_objects_per_page){
						kfree(sl->page[sl->num_pages -1 + count]);
						sl->num_free_objects = sl->num_objects_per_page;
						count++;
					}
				release(&stable.lock);
				sl->num_pages += count;
				return;
				}
			}
		}
		sl->num_pages += count;
	}
	release(&stable.lock);
	return;
}

void slabdump(){
	cprintf("__slabdump__\n");

	struct slab *s;

	cprintf("size\tnum_pages\tused_objects\tfree_objects\n");

	for(s = stable.slab; s < &stable.slab[NSLAB]; s++){
		cprintf("%d\t%d\t\t%d\t\t%d\n", 
			s->size, s->num_pages, s->num_used_objects, s->num_free_objects);
	}
}
