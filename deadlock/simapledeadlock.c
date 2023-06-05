#include <pthread.h> // 线程相关函数
#include <stdio.h> // print函数
#include <unistd.h> // 定义的是linux的基础函数，如sleep函数
pthread_mutex_t r1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t r2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t r3 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t r4 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t r5 = PTHREAD_MUTEX_INITIALIZER;

void *t1_cb(void *arg) {
	pthread_mutex_lock(&r1);
	sleep(1);
	pthread_mutex_lock(&r2);
	pthread_mutex_unlock(&r2);
	pthread_mutex_unlock(&r1);
}

void *t2_cb(void *arg) {
	pthread_mutex_lock(&r2);
	sleep(1);
	pthread_mutex_lock(&r1);
	pthread_mutex_unlock(&r1);
	pthread_mutex_unlock(&r2);
}

void *t3_cb(void *arg) {
	pthread_mutex_lock(&r3);
	sleep(1);
	pthread_mutex_lock(&r4);
	pthread_mutex_unlock(&r4);
	pthread_mutex_unlock(&r3);

}

void *t4_cb(void *arg) {
	pthread_mutex_lock(&r4);
	sleep(1);
	pthread_mutex_lock(&r5);
	pthread_mutex_unlock(&r5);
	pthread_mutex_unlock(&r4);
}

void *t5_cb(void *arg) {
	pthread_mutex_lock(&r5);
	sleep(1);
	pthread_mutex_lock(&r1);
	pthread_mutex_unlock(&r1);
	pthread_mutex_unlock(&r5);

}

// deadlock
int main() {
	pthread_t t1, t2, t3, t4, t5;
	pthread_create(&t1, NULL, t1_cb, NULL);
	pthread_create(&t2, NULL, t2_cb, NULL);
	pthread_create(&t3, NULL, t3_cb, NULL);
	pthread_create(&t4, NULL, t4_cb, NULL);
	pthread_create(&t5, NULL, t5_cb, NULL);

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);
	pthread_join(t3, NULL);
	pthread_join(t4, NULL);
	pthread_join(t5, NULL);
	printf("complete\n");

}