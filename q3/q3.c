#include <stdio.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <errno.h>
#include <pthread.h>
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <semaphore.h>
extern int errno;
typedef long long int ll;
typedef long double ld;
#define f(a,b,i) for(ll i=a;i<b;i++) 
#define BRED "\033[1;31m"
#define LRED "\033[0;31m"
#define BGREEN "\033[1;32m"
#define LGREEN "\033[0;32m"
#define BYELLOW "\033[1;33m"
#define LYELLOW "\033[0;33m"
#define BBLUE "\033[1;34m"
#define LBLUE "\033[0;34m"
#define BMAGENTA "\033[1;35m"
#define LMAGENTA "\033[0;35m"
#define BCYAN "\033[1;36m"
#define LCYAN "\033[0;36m"
#define DEFAULT "\033[0m"


// stage_type = 0 => acoustic
// stage_type = 1 => electric
// status = 0 => free
// status = 1 => musician only
// status = 2 => singer only
// status = 3 => musician + singer


ll n,a,e,c,t1,t2,t;
clock_t initial_time,final_time;
sem_t acoustic_sem,electric_sem,performer_sem,coordinator_sem,time_sem;
pthread_mutex_t mutex1,mutex2,mutex3,mutex4,mutex5;
typedef struct Performer Performer;
typedef struct Coordinator Coordinator;
typedef struct Stage Stage;
int allot[1005];

struct Performer
{
    int id;
    char name[50];
    char instru[2];
    int arrival;
    int arrpri;
    int status;
    int singer;
    int stage_type;
    int stage_numb;
    int performed_flag;
    int shirt;
    int extension;
    int end;
    int timeout;
    int perform_for;
    pthread_t performer_thread_id;
    pthread_t performer_acoustic_thread_id;
    pthread_t performer_electric_thread_id;
    pthread_t coordinator_thread_id;
    pthread_t singer_time_thread_id;
};
struct Stage
{
    int id;
    int status;
    int stage_type;
    int musician_id;
    int singer_id;
};
struct Coordinator
{
    int id;
    int status;
    int performer;
};


Performer performers[1000];
Coordinator coordinators[1000];
Stage stages[1000];
// void * performer_thread(void* args);
void * electric_thread(void* args);
void * acoustic_thread(void* args);
void * singer_thread(void* args);
void * coordinator_thread(void* args);
void * singer_thread(void *args);
void * singer_time_thread(void *args);


int check_acoustic(char *a)
{
    if(a[0]=='p'|| a[0]=='g' || a[0]=='v' || a[0]=='s')
    return 1;
    return 0;
}
int check_electric(char *a)
{
    if(a[0]=='p'|| a[0]=='g' || a[0]=='b' || a[0]=='s')
    return 1;
    return 0;
}
int get_random(int a,int b)
{
    if(a==b)
    return a;
    int r=a+(rand()%(b-a+1));
    return r;
}
void init()
{
    f(0,1005,i)
    allot[i]=0;
}
int main()
{
    init();
    scanf("%lld%lld%lld%lld%lld%lld%lld",&n,&a,&e,&c,&t1,&t2,&t);
    if(n<=0 || (a+e)<=0 || c<=0){
        printf("%s ERROR -> Invalid Input : values of [n], [a+e], [c] must br greater than 0  %s\n",BMAGENTA,DEFAULT);
    return 0;
    }
    if(n>=1000 || (a+e)>=1000 || c>=1000)
    {
        printf("%s ERROR -> Invalid Input : values of [n], [a+e], [c] must br less than 1000  %s\n",BMAGENTA,DEFAULT);
        return 0;
    }
    char temp_name[50];
    char temp_instru[2];
    int temp_arrival;
    f(0,n,i)
    {
        scanf("%s%s%d",temp_name,temp_instru,&temp_arrival);
        performers[i].id=i+1;
        strcpy(performers[i].name,temp_name);
        strcpy(performers[i].instru,temp_instru);
        performers[i].arrival=temp_arrival;
        performers[i].arrpri=0;
        performers[i].singer=0;
        performers[i].status=0;
        performers[i].end=0;
        performers[i].extension=0;
        performers[i].performed_flag=0;
        performers[i].stage_type=0;
        performers[i].stage_numb=0;
        performers[i].perform_for=get_random(t1,t2);
        if(temp_instru[0]=='s') performers[i].singer=1;
        if(check_acoustic(performers[i].instru)==1)
        performers[i].stage_type+=1;
        if(check_electric(performers[i].instru)==1)
        performers[i].stage_type+=2;
    }
    f(0,a,i)
    {
        stages[i].id=i+1;
        stages[i].status=0;
        stages[i].stage_type=0;
    }
    f(a,a+e,i)
    {
        stages[i].id=i+1;
        stages[i].status=0;
        stages[i].stage_type=1;
    }
    f(a,c,i)
    {
        coordinators[i].id=i+1;
        coordinators[i].status=0;
    }
 
    sem_init(&acoustic_sem,0,a);
    sem_init(&electric_sem,0,e);
    sem_init(&performer_sem,0,n);
    sem_init(&time_sem,0,n);
    sem_init(&coordinator_sem,0,c);
    pthread_mutex_init(&mutex1,NULL);
    pthread_mutex_init(&mutex2,NULL);
    pthread_mutex_init(&mutex3,NULL);
    pthread_mutex_init(&mutex4,NULL);
    pthread_mutex_init(&mutex5,NULL);
  
    printf("%s\n=================================================================%s\n",DEFAULT,DEFAULT);
    printf("%sSTARTING SIMULATION %s\n",BMAGENTA,DEFAULT);
    printf("%s-------------------- %s\n",BMAGENTA,DEFAULT);
    struct  timespec tts;
    clock_gettime(CLOCK_REALTIME, &tts);
    initial_time = tts.tv_sec;
    
    for(int i=0; i < n; i++)
    {
        if(performers[i].singer==1)
        {
		    pthread_create(&(performers[i].singer_time_thread_id), NULL, singer_time_thread, &performers[i]);       
		    pthread_create(&(performers[i].performer_thread_id), NULL, singer_thread, &performers[i]);       
        }
        else
        {
            if(performers[i].stage_type%2==1)
            {
                pthread_create(&(performers[i].performer_acoustic_thread_id), NULL, acoustic_thread, &performers[i]);
            }
            if(performers[i].stage_type>1)
            {
                pthread_create(&(performers[i].performer_electric_thread_id), NULL, electric_thread, &performers[i]);
            } 
        }      
    }
		
    for (int i = 0; i < n; i++)
    {
        pthread_join(performers[i].performer_acoustic_thread_id,0);
        pthread_join(performers[i].performer_electric_thread_id,0);
        pthread_join(performers[i].performer_thread_id,0);
        pthread_join(performers[i].coordinator_thread_id,0);
    }  

    printf("%s----------------- %s\n",BMAGENTA,DEFAULT);
    printf("%sSIMULATION OVER %s\n",BMAGENTA,DEFAULT);
		pthread_mutex_destroy(&(mutex1));
		pthread_mutex_destroy(&(mutex2));
		pthread_mutex_destroy(&(mutex3));
		pthread_mutex_destroy(&(mutex4));
		pthread_mutex_destroy(&(mutex5));

    return 0;
}



void * singer_thread(void *args)
{
    Performer * performer = (Performer*)args;
    sleep(performer->arrival);
    printf("%s%s s arrived%s\n",LGREEN,performer->name,DEFAULT);

    // struct timespec ts;
    // clock_gettime(CLOCK_REALTIME, &ts);
    // ts.tv_sec += t;
    // int s1=0;
    // int s2=0;
    // while (((s1 = sem_timedwait(&acoustic_sem, &ts)) == -1 && errno == EINTR) || ((s2 = sem_timedwait(&electric_sem, &ts)) == -1 && errno == EINTR))
    //             continue;       
    // if (s1 == -1 || s2 ==-1)
    // {
    //     if (errno == ETIMEDOUT)
    //     {
    //         pthread_mutex_lock(&mutex5);
    //         if(allot[performer->id]==0){
    //         allot[performer->id]=-1;
    //         performers[performer->id-1].timeout=1;
    //         printf("%s%s with %s leaves impatiently%s\n",LBLUE,performer->name,performer->instru,DEFAULT);
    //         }
    //         pthread_mutex_lock(&mutex5);
    //     }
    //     else
    //         perror("sem_timedwait");
    // } 
    // else
    // {    
    sem_wait(&performer_sem);

    if(performer[performer->id-1].timeout==0)
    {
        int z;
        for(z=0;z<a+e;z++)
        {
            if(stages[z].status==0)
            {
                if(allot[performer->id]==0)
                {
                    allot[performer->id]=1;
                    stages[z].musician_id=performer->id;
                    stages[z].status=2;
                    if(z<a){
                    performers[performer->id-1].arrpri=1;
                    performers[performer->id-1].stage_numb=z+1;
                    }
                    else{
                    performers[performer->id-1].arrpri=2;
                    performers[performer->id-1].stage_numb=z+1-a;
                    }  
                    printf("%s%s performing %s at acoustic stage number %d alone for %d sec%s\n",LRED,performer->name,performer->instru,performers[performer->id-1].stage_numb,performer->perform_for,DEFAULT);
                    break;
                }
                else{
                    sem_post(&performer_sem);
                    return NULL;
                }
            }
        }

        if(z<a+e)
        {
            sleep(performer->perform_for);
            pthread_mutex_lock(&mutex1);
            stages[z].status=0;
            performers[performer->id-1].status=1;
            if(z<a){
            performers[performer->id-1].arrpri=1;
            performers[performer->id-1].stage_numb=z+1;
            }
            else{
            performers[performer->id-1].arrpri=2;
            performers[performer->id-1].stage_numb=z+1-a;
            }           
            printf("%sSolo performance of %s performance at acoustic stage number %d ended%s\n",LYELLOW,performer->name,performers[performer->id-1].stage_numb,DEFAULT);

            pthread_create(&(performers[performer->id-1].coordinator_thread_id),NULL, coordinator_thread, &performers[performer->id-1]);
            pthread_mutex_unlock(&mutex1);
        }

        if(z>=a+e)
        {
            for(z=0;z<a+e;z++)
            {
                if(stages[z].status==1)
                {
                    if(allot[performer->id]==0)
                    {
                        allot[performer->id]=1;
                        stages[z].singer_id=performer->id;
                        stages[z].status=3;
                        pthread_mutex_lock(&mutex4);
                        performers[stages[z].musician_id-1].extension=performer->id;
                        if(z<a){
                        performers[performer->id-1].arrpri=1;
                        performers[performer->id-1].stage_numb=z+1;
                        }
                        else{
                        performers[performer->id-1].arrpri=2;
                        performers[performer->id-1].stage_numb=z+1-a;
                        }    
                        pthread_mutex_unlock(&mutex4);

                        printf("%s%s joined %s's performance, now performance is extended by 2 secs %s\n",BRED,performer->name,performers[stages[z].musician_id-1].name,DEFAULT);
                    }
                    else{
                        sem_post(&performer_sem);
                        return NULL;
                    }
                }
            }
        }

    }

    // }
    sem_post(&performer_sem);
    // if(s1==-1)
    // sem_post(&acoustic_sem);
    // if(s2==-1)
    // sem_post(&electric_sem);
    return NULL;
}

void * acoustic_thread(void* args)
{
	Performer * performer = (Performer*)args;

    sleep(performer->arrival);
    pthread_mutex_lock(&mutex3);
    if(performers[performer->id].arrpri==0)
    {
    printf("%s%s %s arrived%s\n",LGREEN,performer->name,performer->instru,DEFAULT);
    performers[performer->id].arrpri=1;
    }
    pthread_mutex_unlock(&mutex3);


    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += t;
    int s=0;
    while ((s = sem_timedwait(&acoustic_sem, &ts)) == -1 && errno == EINTR)
                continue;       
    if (s == -1)
    {
        if (errno == ETIMEDOUT)
        {
            pthread_mutex_lock(&mutex5);
            if(allot[performer->id]==0){
            allot[performer->id]=-1;
            performers[performer->id-1].timeout=1;
            printf("%s%s with %s leaves impatiently%s\n",LBLUE,performer->name,performer->instru,DEFAULT);
            }
            pthread_mutex_unlock(&mutex5);
            // printf("%s sem_timedwait() timed out\n",performer->name);
        }
        else
            perror("sem_timedwait");
    } 
    else
    { 
        // sem_wait(&acoustic_sem);
        sem_wait(&performer_sem);
            if(performers[performer->id-1].timeout!=1)
            {

                    int z;
                    for(z=0;z<a;z++)
                    {
                        if(stages[z].status==0)
                        {
                            if(allot[performer->id]==0 )
                            {
                                allot[performer->id]=1;
                                performers[performer->id-1].stage_numb=z+1;
                                printf("%s%s performing %s at acoustic stage number %d for %d sec%s\n",LRED,performer->name,performer->instru,performers[performer->id-1].stage_numb,performer->perform_for,DEFAULT);
                                stages[z].musician_id=performer->id;
                                stages[z].status=1;
                                break;
                            }
                            else
                            {
                                sem_post(&performer_sem);
                                sem_post(&acoustic_sem);
                                return NULL;
                            }
                        }
                    }
                    sleep(performer->perform_for);
                    if(performers[performer->id-1].stage_numb!=0 )
                    {
                    if(performers[performer->id-1].end==0)
                    {
                        performers[performer->id-1].end=1;
                        if(performers[performer->id -1].extension!=0)
                        {
                            sleep(2);
                            printf("%s%s and %s performance at acoustic stage number %d ended%s\n",LYELLOW,performer->name,performers[performers[performer->id-1].extension-1].name,performers[performer->id-1].stage_numb,DEFAULT);
                        }
                        else
                        printf("%s%s performance at acoustic stage number %d ended%s\n",LYELLOW,performer->name,performers[performer->id-1].stage_numb,DEFAULT);
                    }
                    pthread_mutex_lock(&mutex1);
                    stages[z].status=0;
                    performers[performer->id-1].status=1;
                    if(performers[performer->id-1].shirt==0)
                    {
                        performers[performer->id-1].shirt=1;
                        pthread_create(&(performers[performer->id-1].coordinator_thread_id),NULL, coordinator_thread, &performers[performer->id-1]);
                    }
                    pthread_mutex_unlock(&mutex1);
                    }
            }

    }
    sem_post(&performer_sem);
    sem_post(&acoustic_sem);
    return NULL;
}

void * electric_thread(void* args)
{
	Performer * performer = (Performer*)args;
    sleep(performer->arrival);
    pthread_mutex_lock(&mutex3);
    if(performers[performer->id].arrpri==0)
    {
    printf("%s%s %s arrived%s\n",LGREEN,performer->name,performer->instru,DEFAULT);
    performers[performer->id].arrpri=2;
    }
    pthread_mutex_unlock(&mutex3);

    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += t;
    int s=0;
    while ((s = sem_timedwait(&electric_sem, &ts)) == -1 && errno == EINTR)
                continue;       
    if (s == -1)
    {
        if (errno == ETIMEDOUT)
        {
            pthread_mutex_lock(&mutex5);
            if(allot[performer->id]==0){
            allot[performer->id]=-1;
            performers[performer->id-1].timeout=1;
            // printf("%s sem_timedwait() timed out\n",performer->name);
            printf("%s%s with %s leaves impatiently%s\n",LBLUE,performer->name,performer->instru,DEFAULT);
            }
            pthread_mutex_unlock(&mutex5);
        }
        else
            perror("sem_timedwait");
    } 
    else
    { 
        // sem_wait(&electric_sem);
        sem_wait(&performer_sem);
            if(performers[performer->id-1].timeout!=1)
            { 
                int z;
                for(z=a;z<e+a;z++)
                    {
                        if(stages[z].status==0)
                        {
                            if(allot[performer->id]==0)
                            {
                                allot[performer->id]=1;
                                performers[performer->id-1].stage_numb=z+1-a;
                                printf("%s%s performing %s at electric stage number %d for %d sec%s\n",LRED,performer->name,performer->instru,performers[performer->id-1].stage_numb,performer->perform_for,DEFAULT);
                                stages[z].musician_id=performer->id;
                                stages[z].status=1;
                                break;
                            }
                            else{
                                sem_post(&performer_sem);
                                sem_post(&electric_sem);
                                return NULL;
                                break;
                            }
                        }
                    }
                    sleep(performer->perform_for);
                    if(performers[performer->id-1].stage_numb!=0 ){
                    if(performers[performer->id -1].extension!=0)
                    {
                        sleep(2);
                        printf("%s%s and %s performance at electric stage number %d ended%s\n",LYELLOW,performer->name,performers[performers[performer->id-1].extension-1].name,performers[performer->id-1].stage_numb,DEFAULT);

                    }
                    else
                    printf("%s%s performance at electric stage number %d ended%s\n",LYELLOW,performer->name,performers[performer->id-1].stage_numb,DEFAULT);
                    pthread_mutex_lock(&mutex1);
                    stages[z].status=0;
                    performers[performer->id-1].status=1;
                    if(performers[performer->id-1].shirt==0)
                    {
                        performers[performer->id-1].shirt=1;
                        pthread_create(&(performers[performer->id-1].coordinator_thread_id),NULL, coordinator_thread, &performers[performer->id-1]);
                    }
                    pthread_mutex_unlock(&mutex1);
                    }
            }
    }
    sem_post(&performer_sem);
    sem_post(&electric_sem);
    return NULL;
}

void * coordinator_thread(void *args)
{
	Performer * performer = (Performer*)args;
    sem_wait(&coordinator_sem);
    sem_post(&performer_sem);
        for(int z=0;z<c;z++)
        {
            if(coordinators[z].status==0)
            {
                coordinators[z].status=1;
                if(performer->status==1)
                {
                    allot[performer->id]=1;
                    sleep(2);
                    printf("%s%s collecting t-shirt from coordinator number %d%s\n",LMAGENTA,performer->name,z+1,DEFAULT);
                    if(performers[performer->id-1].extension!=0)
                    {
                        sleep(2);
                        printf("%s%s collecting t-shirt from coordinator number %d%s  <<-- BONUS \n",LMAGENTA,performers[performers[performer->id-1].extension-1].name,z+1,DEFAULT);

                    }
                coordinators[z].status=0;
                    break;
                }
                else{
                    pthread_mutex_unlock(&mutex2);
                    sem_post(&performer_sem);
                    sem_post(&electric_sem);
                coordinators[z].status=0;
                    return NULL;
                    break;
                }
            }
        }
    sem_post(&performer_sem);
    sem_post(&coordinator_sem);
    return NULL;
}

void * singer_time_thread(void *args)
{
	Performer * performer = (Performer*)args;
    sleep(performer->arrival + t);
    pthread_mutex_lock(&mutex5);
    if(allot[performer->id]==0){
    performers[performer->id-1].timeout=1;
    allot[performer->id]=-1;
    printf("%s%s with s leaves impatiently %s\n",LBLUE,performer->name,DEFAULT);
    }
    pthread_mutex_unlock(&mutex5);
}