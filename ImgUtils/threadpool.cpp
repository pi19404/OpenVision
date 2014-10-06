#include "threadpool.h"




void ThreadPool::cleanup_handler(void * pool1)
{
int rtn=0;
ThreadPool * pool=(ThreadPool *)pool1;
(pool)->log(__FILE__,__FUNCTION__,__LINE__,EDEBUG,"Thread %d enters Cleanup handler . ",pthread_self());
if((rtn=pthread_mutex_unlock(&(pool->queue_lock)))!=0)
{
(pool)->log(__FILE__,__FUNCTION__,__LINE__,EDEBUG,"Cleanup handler activated .Attemptin to unlock mutex .pthread mutex unlock failure. %s. ",strerror(rtn));
}

}


ThreadPool::ThreadPool()
{
    do_not_block_when_full=false;
    SleepTime=10;
    shutdown=false;
    level=WARN;
}


 int ThreadPool::respawn_num_threads(ThreadPool *pool)
{
int ret=0;
pthread_mutex_lock(&(pool->queue_lock));
pool->respawn=pool->respawn+1;
pthread_mutex_unlock(&(pool->queue_lock));
return ret;
}


 int ThreadPool::get_num_respawn_threads(ThreadPool * pool)
{
int ret=0;
pthread_mutex_lock(&(pool->queue_lock));
ret=pool->respawn;
pthread_mutex_unlock(&(pool->queue_lock));
return ret;
}



 int ThreadPool::get_num_threads(ThreadPool *pool)
{
int ret=0;
pthread_mutex_lock(&(pool->queue_lock));
ret=pool->num_threads;
pthread_mutex_unlock(&(pool->queue_lock));
return ret;
}



 int ThreadPool::tpool_cleanup(ThreadPool * pool)
{
int i=0;
int ret=0;
int rtn=0;
int count=0;
int k=0;
while(1)
{
k=0;
for(i=0;i<get_num_threads(pool);i++)
{

if((ret=pthread_equal(pool->threads[i],pthread_self()))==0)
{
( pool)->log(__FILE__,__FUNCTION__,__LINE__,DEBUG,"thread %d Sends cancellation request to thread (%d)(%d) ",pthread_self(),pool->threads[i],ret);
if(pthread_kill(pool->threads[i],0)==0)
{
ret=pthread_cancel(pool->threads[i]);
k++;
if(ret!=0)
{
( pool)->log(__FILE__,__FUNCTION__,__LINE__,EDEBUG,"Error sending thread cancel signal %s ",strerror(ret));
}
}
else
( pool)->log(__FILE__,__FUNCTION__,__LINE__,EDEBUG,"Thread %d has already exited ",pool->threads[i]);
}
}
if(k==0)
{
( pool)->log(__FILE__,__FUNCTION__,__LINE__,ERROR,"Thread cancellation routiene completed successfully ");
break;
}
}
( pool)->log(__FILE__,__FUNCTION__,__LINE__,ERROR,"completed dtpool_cleanup routiene");
return 0;
}




 int ThreadPool::tpool_full_check(ThreadPool * pool)
{
    int rtn = 0;
    int ret = 0;
    if (pool == NULL)
    return -1;
    if ((rtn = pthread_mutex_lock(&pool->queue_lock)) != 0)
    {
   (pool)->log(__FILE__,__FUNCTION__,__LINE__, FATAL, "pthread mutex lock failure. %s.  ", strerror(rtn));
    return -1;
    }
    if ((pool->cur_queue_size == pool->max_queue_size) &&
    (pool->do_not_block_when_full))
    {
    ret = 0;
    }

    else
    ret = 1;
    if ((rtn = pthread_mutex_unlock(&pool->queue_lock)) != 0)
    {
   (pool)->log(__FILE__,__FUNCTION__,__LINE__, FATAL, "pthread mutex unlock failure. %s.  ", strerror(rtn));
    return -1;
    }
    return ret;
}



 int ThreadPool::tpool_add_work(ThreadPool * pool, void (*routine) (void *), void *arg, int arg_size)
{
    int rtn;
    ThreadPoolWork *workp = NULL;

    if (pool == NULL)
    return -1;

    if ((rtn = pthread_mutex_lock(&pool->queue_lock)) != 0)
    {
    (pool)->log(__FILE__,__FUNCTION__,__LINE__, FATAL, "pthread mutex lock failure. %s.  ", strerror(rtn));
    EXIT_THREAD();
    return -1;
    }

    /* now we have exclusive access to the work queue ! */
    if ((pool->cur_queue_size == pool->max_queue_size) && (pool->do_not_block_when_full))
    {
    if ((rtn = pthread_mutex_unlock(&pool->queue_lock)) != 0)
    {
        (pool)->log(__FILE__,__FUNCTION__,__LINE__, FATAL, "pthread mutex lock failure. %s. ", strerror(rtn));
        EXIT_THREAD();
    }
    return -1;
    }

    /* wait for the queue to have an open space for new work, while
     * waiting the queue_lock will be released */
    while ((pool->cur_queue_size == pool->max_queue_size) && (!(pool->shutdown || pool->queue_closed)))
    {
    if ((rtn = pthread_cond_wait(&(pool->queue_not_full), &(pool->queue_lock))) != 0)
    {
       (pool)->log(__FILE__,__FUNCTION__,__LINE__, FATAL, "pthread cond wait failure.%s.  ", strerror(rtn));
        EXIT_THREAD();
    }
    }
    if (pool->shutdown || pool->queue_closed)
    {
    if ((rtn = pthread_mutex_unlock(&pool->queue_lock)) != 0)
    {
       ( pool)->log(__FILE__,__FUNCTION__,__LINE__, FATAL, "pthread mutex lock failure. %s.  ", strerror(rtn));
        EXIT_THREAD();
    }
    return -1;
    }

    /* allocate the work structure */
    if ((workp = new ThreadPoolWork()) == NULL)
    {
   ( pool)->log(__FILE__,__FUNCTION__,__LINE__, FATAL, "unable to create work struct. ");
    return -1;
    }

    /* set the function/routine which will handle the work,
     * (note: it must be reenterant) */
    workp->handler_routine = routine;

    //workp->arg = arg;
    workp->arg = calloc(1,arg_size);
    if(workp->arg==NULL)
    {
       ( pool)->log(__FILE__,__FUNCTION__,__LINE__, FATAL, "unable to create work argument,calloc error");
    if(workp!=NULL)
    free(workp);
        return -1;

    }
    memcpy(workp->arg, arg, arg_size);
    workp->arg_size = arg_size;
    workp->next = NULL;
    if (pool->cur_queue_size == 0)
    {
    pool->queue_tail = pool->queue_head = workp;
    if ((rtn = pthread_cond_broadcast(&(pool->queue_not_empty))) != 0)
    {
        ( pool)->log(__FILE__,__FUNCTION__,__LINE__, FATAL, "pthread broadcast error. %s. ", strerror(rtn));
        if(workp->arg!=NULL)
        free(workp->arg);
        if(workp!=NULL)
        free(workp);
        EXIT_THREAD();
            return -1;
    }
    }

    else
    {
    pool->queue_tail->next = workp;
    pool->queue_tail = workp;
    }
    pool->cur_queue_size++;

    /* relinquish control of the queue */
    if ((rtn = pthread_mutex_unlock(&pool->queue_lock)) != 0)
    {
   ( pool)->log(__FILE__,__FUNCTION__,__LINE__, FATAL, "pthread mutex lock failure. %s. ", strerror(rtn));
        EXIT_THREAD();
    return -1;
    }
    return 0;
}


 int ThreadPool::tpool_destroyEx(ThreadPool * pool)
{
    ThreadPoolWork *cur = NULL;
     /**/ int rtn = 0;
   ( pool)->log(__FILE__,__FUNCTION__,__LINE__, INFO, "destroy pool begin! [ %s] ", pool->poolname.c_str());
    if ((rtn = pthread_mutex_lock(&(pool->queue_lock))) != 0)
    {
   ( pool)->log(__FILE__,__FUNCTION__,__LINE__, FATAL, "pthread mutex lock failure. %s. ", strerror(rtn));
    return -1;
    }

    /* clean up memory */
    if(pool != NULL && pool->threads != NULL) {
        free(pool->threads);
        pool->threads = NULL;
    }
    if(pool!=NULL)
    {
    while (pool->queue_head != NULL)
    {
    cur = pool->queue_head->next;
    pool->queue_head = pool->queue_head->next;
    if(cur != NULL && cur->arg != NULL) {
        free(cur->arg);
        cur->arg = NULL;
    }
    if(cur != NULL) {
        free(cur);
        cur = NULL;
    }
    }
    }
   ( pool)->log(__FILE__,__FUNCTION__,__LINE__, INFO, "destroy pool end! [ %s] ", pool->poolname.c_str());
    if ((rtn = pthread_mutex_unlock(&(pool->queue_lock))) != 0)
    {
   ( pool)->log(__FILE__,__FUNCTION__,__LINE__, FATAL, "pthread mutex unlock failure. %s. ", strerror(rtn));
    return -1;
    }
    if(pool != NULL) {
        free(pool);
        pool = NULL;
    }
    return 0;
}



 int ThreadPool::init(ThreadPool ** pool, string poolname, int num_worker_threads,int max_queue_size, int do_not_block_when_full)
{
    int i, rtn;
    pthread_attr_t attr_thread;
    pthread_mutexattr_t mutex_attr;
    int ReturnBreak=0;
    Logger log;
    log.log(__FILE__,__FUNCTION__,__LINE__, INFO, "init pool  begin [ %s ]...   ", poolname.c_str());

    /* make the thread pool structure */
    if ((*pool = new ThreadPool()) == NULL)
    {
    log.log(__FILE__,__FUNCTION__,__LINE__, FATAL, "Unable to calloc() thread pool! ");
    return -1;
    }

    /* set the desired thread pool values */
    (*pool)->num_threads = num_worker_threads;
    (*pool)->max_queue_size = max_queue_size;
    (*pool)->do_not_block_when_full = do_not_block_when_full;
    (*pool)->num_threads_working = 0;
    if (poolname.empty())
        (*pool)->poolname=string("threadpool");
    else
        (*pool)->poolname=poolname;


    /* create an array to hold a ptr to the worker threads    */
    if (((*pool)->threads = (pthread_t *) calloc(1,sizeof(pthread_t) * num_worker_threads)) == NULL)
    {
        (*pool)->log(__FILE__,__FUNCTION__,__LINE__, FATAL, "Unable to calloc() thread info array. ");
        if((*pool)!=NULL)
        {
                free(*pool);
                (*pool)=NULL;
        }

    return -1;
    }
    (*pool)->log(__FILE__,__FUNCTION__,__LINE__, DEBUG, "initialized threads");
    /* initialize the work queue   */
    (*pool)->cur_queue_size = 0;
    (*pool)->queue_head = NULL;
    (*pool)->queue_tail = NULL;
    (*pool)->queue_closed = 0;
    (*pool)->shutdown = 0;
    (*pool)->respawn =0;

    do
    {

       if((rtn=pthread_mutexattr_init(&mutex_attr))!=0)
        {
                (*pool)->log(__FILE__,__FUNCTION__,__LINE__, FATAL, "pthread_mutex_attr_init %s. ", strerror(rtn));
                ReturnBreak=1;
                break;
        }
        if((rtn=pthread_mutexattr_settype(&mutex_attr,PTHREAD_MUTEX_ERRORCHECK))!=0)
        {
               (*pool)->log(__FILE__,__FUNCTION__,__LINE__, FATAL, "pthread_mutex_attr_init %s. ", strerror(rtn));
               ReturnBreak=1;
               break;
        }

    /* create the mutexs and cond vars   */
    if ((rtn = pthread_mutex_init(&((*pool)->queue_lock),&mutex_attr)) != 0) {
        (*pool)->log(__FILE__,__FUNCTION__,__LINE__, FATAL, "pthread_mutex_init %s. ", strerror(rtn));
        ReturnBreak=1;
        break;
    }
    if ((rtn = pthread_cond_init(&((*pool)->queue_not_empty), NULL)) != 0) {
        (*pool)->log(__FILE__,__FUNCTION__,__LINE__, FATAL, "pthread_cond_init %s. ", strerror(rtn));
        ReturnBreak=1;
        break;
    }
    if ((rtn = pthread_cond_init(&((*pool)->queue_not_full), NULL)) != 0) {
        (*pool)->log(__FILE__,__FUNCTION__,__LINE__, FATAL, "pthread_cond_init %s. ", strerror(rtn));
        ReturnBreak=1;
        break;
    }
    if ((rtn = pthread_cond_init(&((*pool)->queue_empty), NULL)) != 0) {
        (*pool)->log(__FILE__,__FUNCTION__,__LINE__, FATAL, "pthread_cond_init %s. ", strerror(rtn));
        ReturnBreak=1;
        break;
    }

    /*
     * from "man 3c pthread_attr_init"
     * Define the scheduling contention scope for the created thread.  The only
     * value     supported    in    the    LinuxThreads    implementation    is
     * !PTHREAD_SCOPE_SYSTEM!, meaning that the threads contend  for  CPU  time
     * with all processes running on the machine.
     *
     * so no need to explicitly set the SCOPE
     */

    /* create the individual worker threads */
        rtn=pthread_attr_init(&attr_thread);
        if(rtn!=0)
        {
            (*pool)->log(__FILE__,__FUNCTION__,__LINE__, FATAL, "Fatal Error pthread_attr_init %s. ", strerror(rtn));
            ReturnBreak=1;
            break;
        }
        rtn=pthread_attr_setscope(&attr_thread, PTHREAD_SCOPE_SYSTEM);
        if(rtn!=0)
        {
            (*pool)->log(__FILE__,__FUNCTION__,__LINE__, FATAL, "Fatal Error pthread_attr_setscope %d %s. ", rtn,strerror(rtn));
            ReturnBreak=1;
            break;

        }

        unsigned long  old1;
        unsigned long  new1=(4*1024);
        pthread_attr_setstacksize(&attr_thread,new1);
        rtn=pthread_attr_setdetachstate(&attr_thread, PTHREAD_CREATE_JOINABLE);
        if(rtn!=0)
        {
            (*pool)->log(__FILE__,__FUNCTION__,__LINE__, FATAL, "Fatal Error pthread_attr_setdetachstate %s. ", strerror(rtn));
            ReturnBreak=1;
            break;
        }

    for (i = 0; i != num_worker_threads; i++)
    {

    if ((rtn = pthread_create(&((*pool)->threads[i]), &attr_thread,(void* (*)(void*))ThreadPool::tpool_thread, (*pool))) != 0)
    {
        (*pool)->log(__FILE__,__FUNCTION__,__LINE__, FATAL, "pthread_create %s. ", strerror(rtn));
        ReturnBreak=1;
        break;
    }
    (*pool)->log(__FILE__,__FUNCTION__,__LINE__, DEBUG, "pthread_create %d. ", i);
    }
}while(1<0);

        if(ReturnBreak==1)
        {
                if((*pool)->threads!=NULL)
                {
                        free((*pool)->threads);
                        (*pool)->threads=NULL;
                }
                if((*pool)!=NULL)
                {
                        free(*pool);
                        (*pool)=NULL;
                }
            return CM_FAILURE;
    }

    (*pool)->log(__FILE__,__FUNCTION__,__LINE__, INFO, "init pool end [ %s ]! ", poolname.c_str());
    return 0;
}


 void * ThreadPool::tpool_thread(void *tpool)
{
    int rtn=0;
    ThreadPoolWork *my_work = NULL;
    ThreadPool *pool = (ThreadPool *) tpool;
    int state=0;
    int ret=0;
    rtn=pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,&state);
        if(rtn!=0)
        {
               ( pool)->log(__FILE__,__FUNCTION__,__LINE__, FATAL,"Error in pthread_setcancelstate %s ",strerror(rtn));
                pthread_exit(NULL);
        }

    rtn=pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED,&state);
    if(rtn!=0)
    {
               ( pool)->log(__FILE__,__FUNCTION__,__LINE__, FATAL,"Error in pthread_setcanceltype %s ",strerror(rtn));
                pthread_exit(NULL);
    }

    pthread_cleanup_push((void (*)(void*))&ThreadPool::cleanup_handler,(void *)tpool);
    for (;;)
    {				/* go forever */
    pthread_testcancel();
    pthread_mutex_lock(&(pool->queue_lock));



    ( pool)->log(__FILE__,__FUNCTION__,__LINE__, DEBUG, "waiting for work");
    /* sleep until there is work,
     * while asleep the queue_lock is relinquished */
    while ((pool->cur_queue_size == 0) && (!pool->shutdown))
    {
        rtn=pthread_cond_wait(&(pool->queue_not_empty), &(pool->queue_lock));
            if(rtn!=0)
            {
               ( pool)->log(__FILE__,__FUNCTION__,__LINE__, FATAL, "Fatal Error in pthread_cont_wait %s. ", strerror(rtn));
                pthread_cancel(pthread_self());
                pthread_exit(NULL);


            }
    }

    ( pool)->log(__FILE__,__FUNCTION__,__LINE__, DEBUG, "received work");
    /* are we shutting down ?   */
    if (pool->shutdown)
    {
                if ((rtn = pthread_mutex_unlock(&(pool->queue_lock))) != 0)
                {
                       ( pool)->log(__FILE__,__FUNCTION__,__LINE__, FATAL, "pthread mutex unlock failure. %s. ", strerror(rtn));
                }
                ( pool)->log(__FILE__,__FUNCTION__,__LINE__, FATAL, "shutting down thread pool", strerror(rtn));
                pthread_cancel(pthread_self());
                pthread_exit(NULL);
    }

    /* process the work */
    my_work = pool->queue_head;
    pool->cur_queue_size--;
    pool->num_threads_working++;
    if (pool->cur_queue_size == 0)
        pool->queue_head = pool->queue_tail = NULL;

    else
        pool->queue_head = my_work->next;


    \
    if ((!pool->do_not_block_when_full) && (pool->cur_queue_size == (pool->max_queue_size - 1)))
    {
        ( pool)->log(__FILE__,__FUNCTION__,__LINE__, DEBUG, "broadcast that the queue is not full", strerror(rtn));
        if ((rtn = pthread_cond_broadcast(&(pool->queue_not_full))) != 0)
        {
       ( pool)->log(__FILE__,__FUNCTION__,__LINE__, FATAL, "pthread broadcast error. %s. ", strerror(rtn));
                pthread_cancel(pthread_self());
                pthread_exit(NULL);

            }
    }
    if (pool->cur_queue_size == 0)
    {
        if((rtn=pthread_cond_signal(&(pool->queue_empty)))!=0)
        {
       ( pool)->log(__FILE__,__FUNCTION__,__LINE__, FATAL, "pthread cond signal error %s ", strerror(rtn));
                pthread_cancel(pthread_self());
                pthread_exit(NULL);


            }
    }
                if ((rtn = pthread_mutex_unlock(&(pool->queue_lock))) != 0)
                {
                       ( pool)->log(__FILE__,__FUNCTION__,__LINE__, FATAL, "pthread mutex unlock failure. %s. ", strerror(rtn));
                        pthread_exit(NULL);
                }


    ( pool)->log(__FILE__,__FUNCTION__,__LINE__, DEBUG, "executing function");
    /* soket = (int*)my_work->arg;  */
    /* perform the work */
    (*(my_work->handler_routine)) (my_work->arg);
    /*if ((my_work != NULL) && (my_work->arg != NULL)) {
        if(my_work->arg != NULL) {
            free(my_work->arg);
            my_work->arg = NULL;
        }
     }*/
    ( pool)->log(__FILE__,__FUNCTION__,__LINE__, DEBUG, "completed executing function");
    if (my_work != NULL) {
        free(my_work);
        my_work = NULL;
    }
        if ((rtn = pthread_mutex_lock(&(pool->queue_lock))) != 0)
        {
                       ( pool)->log(__FILE__,__FUNCTION__,__LINE__, FATAL, "pthread mutex lock failure. %s. ", strerror(rtn));
                        pthread_exit(NULL);
        }

        pool->num_threads_working--;
        if ((rtn = pthread_mutex_unlock(&(pool->queue_lock))) != 0)
        {
                       ( pool)->log(__FILE__,__FUNCTION__,__LINE__, FATAL, "pthread mutex unlock failure. %s. ", strerror(rtn));
                        pthread_exit(NULL);
        }

        pthread_testcancel();
    }
pthread_cleanup_pop(0);

}
