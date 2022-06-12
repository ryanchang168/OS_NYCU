#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>

int size, n, res = 0, arr[1000000];
typedef struct idx{
	int s;
	int e;
}idx;

float time_diff(struct timeval *start, struct timeval *end){
	return (end->tv_sec - start->tv_sec) + 1e-6*(end->tv_usec - start->tv_usec);
}

void* trd(void *tmp){
	idx *idx_arr = (idx *)tmp;
	pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
	int t = normal((int)idx_arr->s, (int)idx_arr->e);
	pthread_mutex_lock(&mutex);
	res += t;
	pthread_mutex_unlock(&mutex);
	pthread_exit(NULL);
}

int normal(int s, int e){
	int cnt = 0;

	for(int i=s;i<e;i++){
		if(arr[i] == 10)
			cnt++;
	}
	return cnt;
}

int process(int s, int size, int n){
	int seg = size / n;

	pid_t pid[n], tmp;
	for(int i=0;i<n;i++){
		if((pid[i]=fork()) == 0)
			exit(normal(s, s+seg));
		s += seg;
	}
	
	int stat, sum = 0;
	for(int i=0;i<n;i++){
		tmp = wait(&stat);
		if(WIFEXITED(stat))
			sum += WEXITSTATUS(stat);
	}
	return sum;
}

void thread(int s, int e, int n){
	int seg = size/n;
	idx idx_arr;
	idx_arr.s = s;
	idx_arr.e = s+seg;

	for(int i=0;i<n;i++){
		pthread_t t;
		pthread_create(&t, NULL, trd, (void*) &idx_arr);
	        pthread_join(t, NULL);
		idx_arr.s += seg;
		idx_arr.e += seg;
	}	
}

int main(){
	float time = 0.0;
	int cnt = 0;
	for(int i=0;i<1000000;i++)
		arr[i] = 0;
        printf("Size of array? ");
        scanf("%d", &size);
	printf("Number of processes/threads? ");
	scanf("%d", &n);
	printf("----------------------------------\n");
        for(int i=0;i<size;i++)
                arr[i]=rand()%1000;

        struct timeval start, end;
	for(int i=0;i<10;i++){
        	gettimeofday(&start, NULL);
        	cnt = normal(0, size);
        	gettimeofday(&end, NULL);
		time += time_diff(&start, &end);
	}
	printf("Normal Version\n");
        printf("Integer 10 occurs %d times in the array.\n", cnt);
        printf("Time Spent: %0.6f msec\n", time * 1e5);
	printf("-----------------------------------\n");
	
	time = 0.0;
	for(int i=0;i<10;i++){
                gettimeofday(&start, NULL);
                cnt = process(0, size, n);
                gettimeofday(&end, NULL);
                time += time_diff(&start, &end);
        }
	printf("Multi-process Version\n");
        printf("Integer 10 occurs %d times in the array.\n", cnt);
        printf("Time Spent: %0.6f msec\n", time * 1e5);
        printf("-----------------------------------\n");
	
	for(int i=0;i<10;i++){
                gettimeofday(&start, NULL);
                thread(0, size, n);
                gettimeofday(&end, NULL);
                time += time_diff(&start, &end);
        }
	printf("Multi-threaded Version\n");
        printf("Integer 10 occurs %d times in the array.\n", res / 10);
        printf("Time Spent: %0.6f msec\n", time * 1e5);
        printf("-----------------------------------\n");



        return 0;
}

