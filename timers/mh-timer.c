#include <stdio.h>
#include <sys/epoll.h>
#include "mh-timer.h"

// 任务函数
void hello_world(timer_entry_t *te) {
    printf("hello world time = %u\n", te->time);
}

int main() {
    init_timer(); // 初始化定时器。

    //参数1的单位为ms
    add_timer(3000, hello_world); // 向定时器中添加任务。
    add_timer(3000, hello_world); // 向定时器中添加任务。
    add_timer(3001, hello_world); // 向定时器中添加任务。
    add_timer(9000, hello_world); // 向定时器中添加任务。

    int epfd = epoll_create(1);
    struct epoll_event events[512];

    for (;;) {
        int nearest = find_nearest_expire_timer(); // 找到最近到期的任务。
        int n = epoll_wait(epfd, events, 512, nearest); // 成功返回需要处理的事件数目。失败返回0，表示等待超时
        for (int i=0; i < n; i++) {
            // 
        }
        expire_timer();
    }
    return 0;
}

// gcc mh-timer.c minheap.c -o mh -I./