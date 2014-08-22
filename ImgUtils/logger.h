#ifndef LOGGER_H
#define LOGGER_H

#include "Common/OpenCVCommon.hpp"
#include "errno.h"
#include "stdlib.h"
#include "stdio.h"
#include "unistd.h"
#include "semaphore.h"

#include "pthread.h"
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>

#define LOG_FILE_NAME_LENGTH 50
#define LOG_PROGRAM_NAME_LEN 50
#define LOGLINE_MAX 200
#define CM_BUFFER_LENGTH 100

#define EDEBUG 1
#define DEBUG  2
#define INFO   3
#define WARN   4
#define ERROR  5
#define FATAL  6


#define LOG_TRUNC   1<<0
#define LOG_NODATE  1<<1
#define LOG_NOLF    1<<2
#define LOG_NOLVL   1<<3
#define LOG_DEBUG   1<<4
#define LOG_STDERR  1<<5
#define LOG_NOTID   1<<6

class Logger
{
public:
    int fd;
    sem_t sem;
    int flags;
    int level;
    char logfilename[LOG_FILE_NAME_LENGTH];
    char programname[LOG_PROGRAM_NAME_LEN];
    int cur_date;
    int cur_seq;

    int log_open_flag;


    Logger(){
        level=DEBUG;
        log_open_flag=0;
    }

public:

    struct MatchPathSeparator
    {
        bool operator()( char ch ) const
        {
            return ch == '\\' || ch == '/';
        }
    };

    std::string
    basename( std::string  pathname )
    {

        return std::string(std::find_if( pathname.rbegin(), pathname.rend(),MatchPathSeparator() ).base(),pathname.end());
    }


    int log(std::string FILEN,std::string Function,int line1,unsigned int level,char *fmt, ...)
    {

        if (level < this->level)
        {
        return 0;
        }

        int rc;
        va_list ap;
        struct timeval tv;
        time_t curtime;

        char *levels[7] =
        { "[(bad)] ","EDEBUG", "[DEBUG] ", "[INFO ] ", "[WARN ] ", "[ERROR] ", "[FATAL] " };
        string file_name=basename(FILEN);;
        string func_name=Function;

        char buffer[1000];
        char line[1000];
        char line2[100];
        char buf[130]="\0";
        char date[150]="\0";
        int cnt=0;
        sprintf(line2,"-- [%-15s],[%-5d],[%-15s] -- ",file_name.c_str(),line1,func_name.c_str());


        if(log_open_flag!=1)
        {
            memset(date, 0, sizeof(date));
            gettimeofday (&tv, NULL);
            curtime = tv.tv_sec;
            strftime(date, 50, "%b %d %T", localtime(&curtime));
            memset(buf, 0, sizeof(buf));
            strcat(date, buf);
            sprintf(buffer, "(%-5lu) (%-3lu) ",getpid(),(unsigned long) pthread_self());
            cnt = snprintf(line, sizeof(line), "%s%s:%s#", date,line2,buffer);
            va_start(ap, fmt);
            vsnprintf(line + cnt, sizeof(line) - cnt, fmt, ap);
            va_end(ap);
            line[sizeof(line) - 1] = '\0';
        }
        cerr << line << endl;

        /*

        fd = this->fd;


        if (!(this->flags & LOG_NODATE)) {
        memset(date, 0, sizeof(date));
            gettimeofday (&tv, NULL);
            curtime = tv.tv_sec;
            strftime(date, 50, "%b %d %T", localtime(&curtime));
        memset(buf, 0, sizeof(buf));
        strcat(date, buf);
        }


        if (!(this->flags & LOG_NOTID)) {
        sprintf(threadnum, "(%-5lu) (%-3lu) ",getpid(),(unsigned long) pthread_self());
        }

        cnt = snprintf(line, sizeof(line), "%s%s:%s%s",

             mlog->flags & LOG_NODATE ? "" : date,line2, mlog->flags & LOG_NOLVL ? "" : (level >

                              FATAL ? levels[0] : levels[level]), mlog->flags & LOG_NOTID ? "" : threadnum);


        cnt = snprintf(line, sizeof(line), "%s%s:%s",this->flags & LOG_NODATE ? "" : date,line2,this->flags & LOG_NOTID ? "" : threadnum);

        va_start(ap, fmt);
        vsnprintf(line + cnt, sizeof(line) - cnt, fmt, ap);
          va_end(ap);
        line[sizeof(line) - 1] = '\0';

        if (!(this->flags & LOG_NOLF)) {



        }


         strcat(line,"\n");
        rc = write(fd, line, strlen(line));
        if (!rc)
        errno = 0;
        return rc;
        */
        return 0;
    }

    int SToI(const char *s)
    {
        int rVal = 0;
        while (*s != '\0')
        rVal = rVal * 10 + *s++ - '0';
        return rVal;
    }


    int getCurrentDayEx(int *curdayEx)
    {
        char szDay[CM_BUFFER_LENGTH+1]="\0";
        struct tm *ptm = NULL;
        time_t tme;
        tme = time(NULL);
        ptm = localtime(&tme);
        memset(szDay, 0,CM_BUFFER_LENGTH);
        sprintf(szDay, "%d%02d%02d", (ptm->tm_year + 1900), ptm->tm_mon + 1, ptm->tm_mday);
        *curdayEx = SToI(szDay);
        return 0;
    }

    /* log_open
     * params: [fname]                      The name of the logfile to write to
     * params: [flags]                      The bitwise 'or' of zero or more of the following flags:
     *                                      LOG_TRUNC   - Truncates the logfile on opening
     *                                      LOG_NODATE  - Omits the date from each line of the log
     *                                      LOG_NOLF    - Keeps from inserting a trailing '\n' when you don't.
     *                                      LOG_NOLVL   - Keeps from inserting a log level indicator.
     *                                      LOG_STDERR  - Sends log data to stderr as well as to the log.
     *                                      (this not implemented yet)
     * Return:                              NULL on failure, and a valid log_t (value > 0) on success.
     * function:                            Initializes the logfile to be written to with fprintf().
     */
    int log_open(char *fname, char *pname, int flags, char *level)
    {
        struct tm *CT;
        time_t CurrentTime;
        char buffer[LOG_FILE_NAME_LENGTH]="\0";


        this->flags = flags;
        if (strcmp(level, "EDEBUG") == 0)
            this->level = EDEBUG;

        if (strcmp(level, "DEBUG") == 0)
        this->level = DEBUG;

        else if (strcmp(level, "INFO") == 0)
        this->level = INFO;

        else if (strcmp(level, "WARN") == 0)
        this->level = WARN;

        else if (strcmp(level, "ERROR") == 0)
        this->level = ERROR;

        else
        this->level = FATAL;

        if (!strcmp(fname, "-")) {
        this->fd = 2;
        } else {
            time (&CurrentTime);
        CT = localtime (&CurrentTime);
        sprintf(buffer,"%s.%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",fname,CT->tm_year+1900, CT->tm_mon+1, CT->tm_mday,CT->tm_hour,CT->tm_min,CT->tm_sec);
        fd = open(buffer, O_WRONLY | O_CREAT | O_NOCTTY | (flags & LOG_TRUNC ? O_TRUNC : O_APPEND), S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
        strcpy(programname, pname);
        strcpy(logfilename, buffer);
        }
        if (this->fd == -1) {
        fprintf(stderr, "log_open: Opening logfile %s: %s", fname,strerror(errno));
        return -1;
        }
        if (sem_init(&this->sem, 0, 1) == -1) {
        fprintf(stderr, "log_open: Could not initialize log semaphore.");
        return -1;
        }
        if (getCurrentDayEx(&this->cur_date) != 0) {
        fprintf(stderr, "log_open: Could not initialize log cur_date.");
        return -1;
        }
        this->cur_seq = 1;
        log_open_flag=1;
        return 0;
    }


    /* log_close
     * params: [log]                        The log_t corresponding to the log you want to close
     * return: [void]
     * function:                            Closes a logfile when it's no longer needed
     */
    void log_close()
    {
        sem_wait(&sem);
        sem_destroy(&sem);
        close(fd);
        return;
    }


};


#endif // LOGGER_H
