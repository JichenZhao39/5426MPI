//
// Created by JICHEN on 12/3/19.
//
#include <sys/types.h>
#include <sys/wait.h>
#include "errors.h"
#include <pthread.h>

void alarm_fork(void){
    int status;
    char line[128];
    int seconds;
    pid_t pid;
    char message[64];

    while (1){
        printf("Alarm > ");
        if (fgets(line, sizeof(line),stdin) == NULL)
            exit(0);
        if (strlen(line) <= 1)
            continue;

        /*
         * Parse input line into seconds (%d) and a message(%64[^\n],)
         * consisting of up to 64 characters separated from the seconds by whitespace
         */
        if (sscanf(line,"%d %64[^\n]",&seconds,message) < 2){
            fprintf(stderr,"Bad command\n");
        } else{
            pid = fork();
            if (pid == (pid_t) - 1)
                errno_abort("Fork");
            if (pid == (pid_t)0){
                //In the child, wait and then print a message
                sleep(seconds);
                printf("(%d) %s\n",seconds,message);
                exit(0);
            } else{
                //in the parent, call waitpid() to collecct children
                //that have already terminated
                do{
                    pid = waitpid((pid_t)-1,NULL,WNOHANG);
                    if (pid == (pid_t)-1)
                        errno_abort("Wait for child");
                }while (pid != (pid_t)0);
            }
        }
    }
}

typedef struct alarm_tag{
    int seconds;
    char message[64];
} alarm_t;

void *alarm_threads(void *arg){
    alarm_t *alarm = (alarm_t*)arg;
    int status;
    //调用pthread_detach函数来分离自己
    status = pthread_detach(pthread_self());
    if (status != 0)
        err_abort(status,"Deach thread");
    sleep(alarm ->seconds);
    printf("(%d) %s\n",alarm->seconds,alarm->message);
    free(alarm);
    return NULL;
}

int alarm_thread_main(void){
    int status;
    char line[128];
    alarm_t *alarm;
    //线程标识符
    pthread_t thread;

    while (1){
        printf("Alarm > ");
        if (fgets(line, sizeof(line),stdin) == NULL)
            exit(0);
        if (strlen(line) <= 1)
            continue;
        alarm = (alarm_t *)malloc(sizeof(alarm_t));
        if (alarm == NULL)
            errno_abort("Allocate alarm");
        /*
         * Parse input line into seconds (%d) and a message(%64[^\n],)
         * consisting of up to 64 characters separated from the seconds by whitespace
         */
        if (sscanf(line,"%d %64[^\n]",&alarm->seconds,alarm->message) < 2){
            fprintf(stderr,"Bad command\n");
            free(alarm);
        } else{
            status = pthread_create(&thread,NULL,alarm_threads,alarm);
            if (status != 0)
                err_abort(status,"Create alarm thread");
        }
    }
    return 0;
}

//thread start routine
//* 表示函数返回一个地址值。
void *thread_routine(void *arg){
    return arg;
}
int lifecycle(void){
    pthread_t thread_id;
    void *thread_result;
    int status;

    status = pthread_create(&thread_id,NULL,thread_routine,NULL);
    if (status != 0)
        err_abort(status,"Create thread");

    status = pthread_join(thread_id,&thread_result);
    if (status != 0){
        err_abort(status,"Join thread");
    }
    if (thread_result == NULL)
        return 0;
    else
        return 1;
}
/*
 * The 'alarm' structure now contains the time_t (time since the epoch,inseconds)
 * for each alarm,so that they can be sorted. Storing the requested number of seconds
 * would not be enough,since the 'alarm thread' cannot tell how long it has
 * been on the list
 */
typedef struct alarm_tags{
    struct alarm_tags *link;
    int seconds;
    time_t time;    //seconds from EPOCH
    char message[64];
} alarm_ta;

pthread_mutex_t alarm_mutex = PTHREAD_MUTEX_INITIALIZER;
alarm_ta *alarm_list = NULL;

//the alarm thread's start routine
void *alarm_thread(void *arg){
    alarm_ta *alarm;
    int sleep_time;
    time_t now;
    int status;
    //loop forever, processing commands. The alarm thread will be disintegrated
    //when the process exits

    while (1){
        status = pthread_mutex_lock(&alarm_mutex);
        if (status != 0)
            err_abort(status,"Lock mutex");
        alarm = alarm_list;
        /*
         * if the alarm list is empty wait for one second.this allows
         * the main thread to run and read another command. If the list is not
         * empty,remove the first item. compute the number of seconds to wait
         * if the result is less than 0 (the time has passed). then set
         * the sleep_time to 0
         */
        if (alarm == NULL)
            sleep_time = 1;
        else{
            alarm_list = alarm->link;
            now = time(NULL);
            if (alarm->time <= now)
                sleep_time = 0;
            else
                sleep_time = alarm->time - now;
#ifdef DEBUG
            printf("[waiting: %d(%d)\"%s\"]\n",alarm->time,sleep_time,
                    alarm->message);
#endif
        }
        /*
         * Unlock the mutex before waiting,so that the main thread can
         * lock it to insert a new alarm request. if the sleep_time is 0,
         * then call sched_yield,giving then main thread a chance to run
         * if it has been readied by user input,without delaying the message
         * if there's no input
         */
        status = pthread_mutex_lock(&alarm_mutex);
        if (status != 0)
            err_abort(status,"Unlock mutex");
        if (sleep_time > 0)
            sleep(sleep_time);
        else
            sched_yield();

        //if a timer expired,print the message and free the structure
        if (alarm != NULL){
            printf("(%d) %s\n",alarm->seconds,alarm->message);
            free(alarm);
        }
    }

}

int alarm_mutex_main(void){
    int status;
    char line[128];
    alarm_ta *alarm,**last,*next;
    pthread_t thread;
    status = pthread_create(&thread,NULL,alarm_thread,NULL);
    if (status != 0)
        err_abort(status,"Create alarm thread");
    while (1){
        printf("Alarm > ");
        if (fgets(line, sizeof(line),stdin) == NULL)
            exit(0);
        if (strlen(line) <= 1)
            continue;
        alarm = (alarm_t*)malloc(sizeof(alarm_t));
        if (alarm == NULL)
            errno_abort("Allocate alarm");

        /*
         * Parase input line into seconds(%d) and a message(%64[^\n])
         * consisting of up to 64 characters
         * separated from the seconds by whitespace
         */
        if (sscanf(line,"%d %64[^\n]",
                &alarm->seconds,alarm->message)<2){
            fprintf(stderr,"Bad command\n");
            free(alarm);
        } else{
            status = pthread_mutex_lock(&alarm_mutex);
            if (status != 0)
                err_abort(status,"Lock mutex");
            alarm->time = time(NULL) + alarm->seconds;
            /*
             * insert the new alarm into the list of alarms
             * sorted by expiration time
             */
            last = &alarm_list;
            next = *last;
            while (next != NULL){
                if (next->time >= alarm->time){
                    alarm->link = next;
                    *last = alarm;
                    break;
                }
                last = &next->link;
                next = next->link;
            }
            /*
             * if we reached the end of the list,insert the new alarm there.
             * ('next' is NULL and 'last' points to the link field of the last
             * item, or to the list header)
             */
            if (next == NULL){
                *last = alarm;
                alarm->link = NULL;
            }
#ifdef DEBUG
            printf ("[list: ");
            for (next = alarm_list; next != NULL; next = next->link)
                printf ("%d(%d)[\"%s\"] ", next->time,
                    next->time - time (NULL), next->message);
            printf ("]\n");
#endif
            status = pthread_mutex_unlock(&alarm_mutex);
            if (status != 0)
                err_abort(status,"Unlock mutex");
        }
    }
}

//define a structure, with a mutex
typedef struct my_struct_tag{
    pthread_mutex_t mutex;  //protects access to value
    int             value;  //Access protected by mutex
} my_struct_t;
int mutext_dynamic(void){
    my_struct_t *data;
    int status;
    data = malloc(sizeof(my_struct_t));
    if (data == NULL)
        errno_abort("Allocate structure");
    status = pthread_mutex_init(&data->mutex,NULL);
    if (status != 0)
        err_abort(status,"Init mutex");
    status = pthread_mutex_destroy(&data->mutex);
    if (status != 0)
        err_abort(status,"Destroy mutex");
    (void)free(data);
    return status;

}
#define SPIN 10000000

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
long counter;
time_t end_time;
/*
 * Thread start routine that repeatedly locks a mutex and increments a counter
 */
void *counter_thread(void *arg){
    int status;
    int spin;
    /*
     * Until end_time,increamet the counter each second.Instead of just increamenting
     * the counter, it sleeps for another second with the mutex locked,to give monitor_thread
     * a reasonable chance of running
     */
    while (time(NULL) < end_time){
        status = pthread_mutex_lock(&mutex);
        if (status != 0)
            err_abort(status,"Lock mutex");
        for (int spin = 0; spin < SPIN; spin++) {
            counter++;
        }
        status = pthread_mutex_unlock(&mutex);
        if (status != 0)
            err_abort(status,"Unlock mutex");
        sleep(1);
    }
    printf("Counter is %#1x\n",counter);
    return NULL;
}
/*
 * Thread start routine to "monitor" the counter. Every 3 seconds try to lock the mutex and
 * read the counter.If the trylock fails.skip this cycle
 */
void *monitor_thread(void *arg){
    int status;
    int misses = 0;
    //loop until end_time checking the counter every 3 seconds
    while (time(NULL) < end_time){
        sleep(3);
        status = pthread_mutex_trylock(&mutex);
        if (status != EBUSY){
            if (status != 0)
                err_abort(status,"Trylock mutex");
            printf("Counter is %ld\n",counter/SPIN);
            status = pthread_mutex_unlock(&mutex);
            if (status != 0)
                err_abort(status,"Unlock mutex");
        } else
            misses++;       //count 'misses' on the lock
    }
    printf("Monitor thread missed update %d times. \n",misses);
    return NULL;
}
int trylock(void){
    int status;
    pthread_t counter_thread_id;
    pthread_t monitor_thread_id;
#ifdef sun
    /*
     * to ensure that our threads can run concurrently. we need to increase
     * the concurrency level to 2
     */
    DPRINTF(("Setting concurrency level to 2\n"));
    thr_setconcurrency(2);
#endif
    end_time = time(NULL)+60;   //run for 1 minute
    status = pthread_create(&counter_thread_id,NULL,counter_thread,NULL);
    if (status != 0)
        err_abort(status,"Create counter thread");
    status = pthread_create(&monitor_thread_id,NULL,monitor_thread,NULL);
    if (status != 0)
        err_abort(status,"create monitor thread");
    status = pthread_join(counter_thread_id,NULL);
    if (status != 0)
        err_abort(status,"Join counter thread");
    status = pthread_join(monitor_thread_id,NULL);
    if (status != 0)
        err_abort(status,"Join monitor thread");
    return 0;
}

#define ITERATIONS 10
//Initialize a static array of 3 mutexes
pthread_mutex_t mutex1[3] = {
        PTHREAD_MUTEX_INITIALIZER,
        PTHREAD_MUTEX_INITIALIZER,
        PTHREAD_MUTEX_INITIALIZER
};
int backoff = 1;    //whether to backoff or deadlock
int yield_flag = 0; //0:yield,>0yield,<0 sleep
/*
 * this is a thread start routine that locks all mutex in order
 * to ensure a conflic with lock_reverse,whidh does opposite
 */
void *look_forward(void *arg){
    int i,iterate,backoffs;
    int status;

    for (int iterate = 0; iterate < ITERATIONS; iterate++) {
        backoffs = 0;
        for (int i = 0; i < 3; i++) {
            if (i == 0){
                status = pthread_mutex_lock(&mutex1[i]);
                if (status != 0)
                    err_abort(status,"First lock");
            } else{
                if (backoff)
                    status = pthread_mutex_trylock(&mutex1[i]);
                else
                    status = pthread_mutex_lock(&mutex1[i]);
                if (status == EBUSY){
                    backoffs++;
                    DPRINTF(("[forward locker backing off at %d]\n",i));
                    for (;i>=0;i--){
                        status = pthread_mutex_unlock(&mutex1[i]);
                        if (status != 0)
                            err_abort(status,"Backoff");
                    }
                } else{
                    if (status != 0)
                        err_abort(status,"Lock mutex");
                    DPRINTF((" forward locker got %d\n",i));
                }
            }
            /*
             * Yeild processor if needed to be sure locks get interleaved on
             * a uniprocessor
             */
            if (yield_flag){
                if (yield_flag >0)
                    sched_yield();
                else
                    sleep(1);
            }
        }
        //report that we got 'em and unlock to try aagin
        printf("lock forward got all locks, %d backoffs\n",backoffs);
        pthread_mutex_unlock(&mutex1[2]);
        pthread_mutex_unlock(&mutex1[1]);
        pthread_mutex_unlock(&mutex1[0]);
        sched_yield();
    }
    return NULL;
}
/*
 * this is a thread start routine that locks all mutexes in reverse order,
 * to ensure a confilct with lock_foward which does the opposite
 */
void *lock_backward(void *args){
    int i,iterate,backoffs;
    int status;
    for (int iterate = 0; iterate < ITERATIONS; iterate++) {
        backoffs = 0;
        for (int i = 2; i >=0 ; i--) {
            if (i == 2){
                status = pthread_mutex_lock(&mutex1[i]);
                if (status != 0)
                    err_abort(status,"First lock");
            } else{
                if (backoff)
                    status = pthread_mutex_trylock(&mutex1[i]);
                else
                    status = pthread_mutex_lock(&mutex1[i]);
                if (status == EBUSY){
                    backoffs++;
                    DPRINTF(("[backward locker backing off at %d]\n",i));
                    for (;i<3;i++){
                        status = pthread_mutex_unlock(&mutex1[i]);
                        if (status != 0)
                            err_abort(status,"Backoff");
                    }
                } else{
                    if (status != 0)
                        err_abort(status,"Lock mutex");
                    DPRINTF(("backward locker got %d\n",i));
                }
            }
            /*
             * yield processor if needed to be sure locks get
             * interleaved on a uniprocessor
             */
            if (yield_flag){
                if (yield_flag > 0)
                    sched_yield();
                else
                    sleep(1);
            }
        }
        //report that we got 'em, and unlock to try again
        printf("lcok backward got all locks, %d backoffs\n",backoffs);
        pthread_mutex_unlock(&mutex1[0]);
        pthread_mutex_unlock(&mutex1[1]);
        pthread_mutex_unlock(&mutex1[2]);
        sched_yield();
    }
    return NULL;
}

int main(int argc, char *argv[]){

    //alarm_fork();
    //alarm_thread_main();
    //lifecycle();
    //alarm_thread_main();
    //mutext_dynamic();
    //trylock();
    pthread_t forward,backward;
    int status;
#ifdef sun
    //to ensure that our thread can run concurrently,we need to increase the concurerency level
    DPRINTF(("Setting concurrency level to 2\n"));
    thr_setconcurrency(2);
#endif
    /*
     * if the first argument is absent or nonzero a backoff algorithm will be used to
     * avoid deadlock. if the first argument is zero,the program will
     * deadlock on a lock 'collision'
     */
    if (argc>1)
        backoff = atoi(argv[1]);

    /*
     * if the second argument is absent or zero,the two threads run 'at speed'
     * on some system,especially uniprocessors, one thread may complete before
     * the other has a chance to run, and you won't see a deadlock or backoffs
     * in that case,try running with the argument set to a posiive number to cause the threads
     * to call sched_yield at each lock;or to make it even more obvious,set to a negative number to
     * cause the threads to call sleep(1) instead
     */
    if (argc > 2)
        yield_flag = atoi(argv[2]);
    status = pthread_create(&forward,NULL,lock_backward,NULL);
    if (status != 0)
        err_abort(status,"Create forward");
    status = pthread_create(&backward,NULL,lock_backward,NULL);
    if (status != 0)
        err_abort(status,"Create backward");
    pthread_exit(NULL);
    return 0;
}
