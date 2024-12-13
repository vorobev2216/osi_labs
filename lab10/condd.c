#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define ARRAY_SIZE 10
#define NUM_READERS 10

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

int shared_array[ARRAY_SIZE];
int write_count = 0;

void* writer_thread(void* arg) {
    while (write_count < ARRAY_SIZE) {
	usleep(100000);
        pthread_mutex_lock(&mutex);
        

        if (write_count < ARRAY_SIZE) {
            shared_array[write_count] = write_count;
            printf("Writer updated array at index: %d\n", shared_array[write_count]);
            write_count++;
	    pthread_cond_broadcast(&cond);
        }
		
        pthread_mutex_unlock(&mutex);
        
    }
    return NULL;
}

void* reader_thread(void* arg) {
    int tid = *((int*)arg);
    while(write_count < ARRAY_SIZE) {
        pthread_mutex_lock(&mutex);
	pthread_cond_wait(&cond, &mutex);


        printf("Reader %d reads array: ", tid);
        for (int i = 0; i < write_count; i++) {
            printf("%d ", shared_array[i]);
        }
        printf(", tid: [%lx]\n", pthread_self());

        if (write_count >= ARRAY_SIZE) {
            pthread_mutex_unlock(&mutex);
            break;
        }
        
        pthread_mutex_unlock(&mutex);

    }
    return NULL;
}

int main() {
    pthread_t readers[NUM_READERS];
    pthread_t writer;
    int reader_ids[NUM_READERS];

    if (pthread_mutex_init(&mutex, NULL) != 0) {
        fprintf(stderr, "Error initializing mutex: %s\n", strerror(errno));
        return 1;
    }

    if (pthread_create(&writer, NULL, writer_thread, NULL) != 0) {
        fprintf(stderr, "Error creating writer thread: %s\n", strerror(errno));
        pthread_mutex_destroy(&mutex);
        return 1;
    }

    for (int i = 0; i < NUM_READERS; i++) {
        reader_ids[i] = i;
        if (pthread_create(&readers[i], NULL, reader_thread, &reader_ids[i]) != 0) {
            fprintf(stderr, "Error creating reader thread %d: %s\n", i, strerror(errno));
            pthread_cancel(writer);
            pthread_mutex_destroy(&mutex);
            return 1;
        }
    }

    for (int i = 0; i < NUM_READERS; ++i) {
        void* res = NULL;
        int join_res = pthread_join(readers[i], &res);
        if (join_res != 0) {
            int err = errno;
            printf("ERROR IN JOIN (reader %d): %s(%d)\n", i, strerror(err), err);
        }
    }

    void* res = NULL;
    int join_res = pthread_join(writer, &res);
    if (join_res != 0) {
        printf("ERROR IN JOIN (writer): %s\n", strerror(errno));
    }

    pthread_mutex_destroy(&mutex);
    return 0;
}