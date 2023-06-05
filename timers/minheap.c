#include "minheap.h"

#define min_heap_elem_greater(a, b) \
    ((a)->time > (b)->time)

// 创建最小堆
void min_heap_ctor_(min_heap_t* s) { s->p = 0; s->n = 0; s->a = 0; }

// 
void min_heap_dtor_(min_heap_t* s) { if (s->p) free(s->p); }
void min_heap_elem_init_(timer_entry_t* e) { e->min_heap_idx = -1; }
int min_heap_empty_(min_heap_t* s) { return 0u == s->n; }
unsigned min_heap_size_(min_heap_t* s) { return s->n; }
timer_entry_t* min_heap_top_(min_heap_t* s) { return s->n ? *s->p : 0; }

int min_heap_push_(min_heap_t* s, timer_entry_t* e)
{
    if (min_heap_reserve_(s, s->n + 1))
        return -1; // 如果空间分配失败，则返回，程序终止。
    // 插入新任务，并且上升调整。
    min_heap_shift_up_(s, s->n++, e);
    return 0;
}

timer_entry_t* min_heap_pop_(min_heap_t* s)
{
    if (s->n)
    {
        timer_entry_t* e = *s->p;
        // s->p[--s->n]是该堆的最后一个元素，相当于将最后一个元素填入到最小元素的位置，而后进行下降操作。
        min_heap_shift_down_(s, 0u, s->p[--s->n]);
        e->min_heap_idx = -1;
        return e;
    }
    return 0;
}

int min_heap_elt_is_top_(const timer_entry_t *e)
{
    return e->min_heap_idx == 0;
}

int min_heap_erase_(min_heap_t* s, timer_entry_t* e)
{
    if (-1 != e->min_heap_idx)
    {
        timer_entry_t *last = s->p[--s->n];
        unsigned parent = (e->min_heap_idx - 1) / 2;
        /* we replace e with the last element in the heap.  We might need to
           shift it upward if it is less than its parent, or downward if it is
           greater than one or both its children. Since the children are known
           to be less than the parent, it can't need to shift both up and
           down. */
        if (e->min_heap_idx > 0 && min_heap_elem_greater(s->p[parent], last))
            min_heap_shift_up_unconditional_(s, e->min_heap_idx, last);
        else
            min_heap_shift_down_(s, e->min_heap_idx, last);
        e->min_heap_idx = -1;
        return 0;
    }
    return -1;
}

int min_heap_adjust_(min_heap_t *s, timer_entry_t *e)
{
    if (-1 == e->min_heap_idx) {
        return min_heap_push_(s, e);
    } else {
        unsigned parent = (e->min_heap_idx - 1) / 2;
        /* The position of e has changed; we shift it up or down
         * as needed.  We can't need to do both. */
        if (e->min_heap_idx > 0 && min_heap_elem_greater(s->p[parent], e))
            min_heap_shift_up_unconditional_(s, e->min_heap_idx, e);
        else
            min_heap_shift_down_(s, e->min_heap_idx, e);
        return 0;
    }
}

int min_heap_reserve_(min_heap_t* s, unsigned n)
{
    // 当需要插入新的任务时，如果当前容量小于插入后的任务数量，执行如下if操作，申请内存空间。
    if (s->a < n)
    {
        // 相当于声明了一个*p[]，即一个指向一个timer_entry_t数组的指针。
        timer_entry_t** p; 
        // 若容量==0，初始申请8个节点，若容量!=0，初始申请原来容量的2倍。
        unsigned a = s->a ? s->a * 2 : 8;
        // [MJ]不知道干嘛的？
        if (a < n)
            a = n;

        // 重新开辟一个容量为a的一维数组，数组中的每个元素指向一个timer_entry_s。
        if (!(p = (timer_entry_t**)realloc(s->p, a * sizeof *p)))
            return -1;
        
        // 将新开辟的地址赋值给min_heap_t中指向的一维数组的地址。
        s->p = p;
        // 更新容量。
        s->a = a;
    }
    return 0;
}

void min_heap_shift_up_unconditional_(min_heap_t* s, unsigned hole_index, timer_entry_t* e)
{
    unsigned parent = (hole_index - 1) / 2;
    do
    {
    (s->p[hole_index] = s->p[parent])->min_heap_idx = hole_index;
    hole_index = parent;
    parent = (hole_index - 1) / 2;
    } while (hole_index && min_heap_elem_greater(s->p[parent], e));
    (s->p[hole_index] = e)->min_heap_idx = hole_index;
}

void min_heap_shift_up_(min_heap_t* s, unsigned hole_index, timer_entry_t* e)
{
    // hole_index:新插入元素所在的位置下标
    unsigned parent = (hole_index - 1) / 2;
    while (hole_index && min_heap_elem_greater(s->p[parent], e))
    {
        // 如果父节点的值>新插入节点的值，则将父节点下沉，插入的子节点上升；
        (s->p[hole_index] = s->p[parent])->min_heap_idx = hole_index;
        hole_index = parent;
        parent = (hole_index - 1) / 2;
    }
    // 最后将新插入的节点放入到对应的位置，并且设置对应的在堆中的index
    (s->p[hole_index] = e)->min_heap_idx = hole_index;
}

void min_heap_shift_down_(min_heap_t* s, unsigned hole_index, timer_entry_t* e)
{
    // hole_index 为开始下沉的位置。e是该堆的最后一个元素

    unsigned min_child = 2 * (hole_index + 1); // 右孩子
    while (min_child <= s->n)
    {
        // 如果当前min_child是堆的最后一个值或者当前值比左子树值大，则min_child指向左子树，也就是指向小的。
        min_child -= min_child == s->n || min_heap_elem_greater(s->p[min_child], s->p[min_child - 1]);
        // 如果当前层的值都比最后一个元素大，则不需要下沉了，退出循环
        if (!(min_heap_elem_greater(e, s->p[min_child])))
            break;
        (s->p[hole_index] = s->p[min_child])->min_heap_idx = hole_index;
        hole_index = min_child;
        min_child = 2 * (hole_index + 1);
    }
    // 将最后一个元素填入到合适的位置，完成下沉。
    (s->p[hole_index] = e)->min_heap_idx = hole_index;
}