#ifndef THREADPOOL_H
#define THREADPOOL_H
#include "ImgUtils/logger.h"
#include "signal.h"
#define CM_FAILURE -1



class ThreadPoolWork
{
public:
    void (*handler_routine) (void *);
    void *arg;
    int arg_size;
    ThreadPoolWork *next;
};



class ThreadPool : public Logger
{
public:



#define EXIT_THREAD() \
        int state; \
                pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,&state); \
                pthread_cancel(pthread_self()); \
        pthread_exit(NULL);


    void cleanup_handler(void * pool1);
    static int tpool_full_check(ThreadPool * pool);
    static int tpool_add_work(ThreadPool * pool, void (*routine) (void *), void *arg, int arg_size);



    ThreadPool();

    int SleepTime;
    string poolname;
    int num_threads;
    int max_queue_size;
    int num_threads_working;
    int do_not_block_when_full;
    int cur_queue_size;
    pthread_t *threads;

    pthread_mutex_t queue_lock;

    pthread_cond_t queue_not_full;
    pthread_cond_t queue_not_empty;
    pthread_cond_t queue_empty;
    int queue_closed;
    int shutdown;
    int respawn;

    ThreadPoolWork *queue_head;
    ThreadPoolWork *queue_tail;


    static int respawn_num_threads(ThreadPool *pool);
    static int get_num_respawn_threads(ThreadPool * pool);
    static int get_num_threads(ThreadPool *pool);
    static int tpool_cleanup(ThreadPool * pool);





    static int tpool_destroyEx(ThreadPool * pool);


    static void *tpool_thread(void *tpool);


    static int init(ThreadPool ** pool, string poolname, int num_worker_threads,int max_queue_size, int do_not_block_when_full);



};





#endif // THREADPOOL_H
