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

char *round_arr[]= { "1st","2nd","3rd"};
ll n, o, m;
typedef struct Company Company;
typedef struct Zone Zone;
typedef struct Student Student;
struct Company
{
	int id;
    int batch_left;
    int capacity;
    ld prob;
	pthread_mutex_t company_mutex;
    pthread_t company_thread_id;
    pthread_cond_t cond_var_zone; 
};
struct Zone
{
    int id;
    int capacity;
    int slots;
    int occupancy;
    ld prob;
    pthread_mutex_t zone_mutex;
    pthread_t zone_thread_id;
    pthread_cond_t cond_var_student;
};
struct Student
{
    int id;
    int zone_id;
    int chance;
    int goin;
    pthread_t student_thread_id;
    pthread_mutex_t student_mutex;

};
Company companys[1000]; Zone zones[1000]; Student students[1000]; 
int numb_waiting;
pthread_mutex_t waiting_student;
void report();
void * company_thread(void* args);
void * zone_thread(void* args);
void * student_thread(void* args);
void slot_allotment(int id);
void student_in_slot(int i, int id);
void manufacture_vaccine(Company *company);
void slot_check(Zone *zone);
int get_prob(double a);
int get_random(int a,int b);
int min(int a, int b);

int main()
{
    srand(time(0));
    printf("Enter number of pharmaceutical company : "); 
    scanf("%lld",&n);
    printf("Enter number of vaccination zones : "); 
    scanf("%lld",&m);
    printf("Enter number of students : "); 
    scanf("%lld",&o);
    if(n>=1000 || m>=1000 || o>=1000)
    {
        printf("%sInvalid input : Input must be less than `1000`%s\n",BRED,DEFAULT);
        return 0;
    }
    ld p[n];

    printf("Enter probabilities of success rate of all companies : \n");
    f(0,n,i) scanf("%Lf",&p[i]);

    if(n<=0 || m<=0 || o<=0)
    {
        printf("%sInvalid input : Input must be greater than `0`%s\n",BRED,DEFAULT);
        return 0;
    }
   
    numb_waiting=0;
    pthread_mutex_init(&(waiting_student), NULL);


    for(int i=0; i < n; i++)
    {
        companys[i].id = i+1;
        companys[i].prob=p[i];
        pthread_mutex_init(&(companys[i].company_mutex), NULL);
    }   

    for(int i=0; i < m; i++)
    {
        zones[i].id = i+1;
        pthread_mutex_init(&(zones[i].zone_mutex), NULL);
    }  

    for(int i=0; i < o; i++)
    {
        students[i].id = i+1;
        students[i].chance = 0;
        students[i].goin = 0;
        students[i].zone_id=-1;
        pthread_mutex_init(&(students[i].student_mutex), NULL);
    }

    printf("%s\n=================================================================%s\n",DEFAULT,DEFAULT);
    printf("%sSTARTING SIMULATION %s\n",BMAGENTA,DEFAULT);
    printf("%s------------------------------------------ %s\n",BMAGENTA,DEFAULT);

    for(int i=0; i < n; i++)
		pthread_create(&(companys[i].company_thread_id), NULL, company_thread , &companys[i]);

	for(int i=0; i < m; i++)
		pthread_create(&(zones[i].zone_thread_id), NULL, zone_thread , &zones[i]);

    for(int j=0;j<3;j++)
    {
        for(int i=0; i < o; i++)
        if(students[i].chance==j)
		pthread_create(&(students[i].student_thread_id), NULL, student_thread , &students[i]);
    }
    
	for(int i=0; i < o; i++)
		pthread_join(students[i].student_thread_id, 0);  

    printf("%s------------------------------------------ %s\n",BMAGENTA,DEFAULT);
    printf("%sSIMULATION OVER %s\n",BMAGENTA,DEFAULT);

    for(int i=0; i< n; i++)
		pthread_mutex_destroy(&(companys[i].company_mutex));
    for(int i=0; i < m; i++)
		pthread_mutex_destroy(&(zones[i].zone_mutex));

    report();
	return 0; 
}

int get_random(int a,int b)
{
    if(a==b || b==0)
    {
        return a;
    }
    int r=a+(rand()%(b-a+1));
    return r;
}

int get_prob(double a)
{
    double r = (double)rand() / RAND_MAX;
    if(r<=a)
    return 1;
    return 0;
}

int min(int a, int b)
{
    if(a<b)
    return a;
    return b;
}

void report()
{
    printf("%s=================================================================%s\n",DEFAULT,DEFAULT);

    printf("%sREPORT AFTER THE SIMULATION%s\n",BGREEN,DEFAULT);
    printf("%s------------------------------%s\n",BGREEN,DEFAULT);
    int kick=0;
    printf("%sStudents sent home for another online semester%s\n",BBLUE,DEFAULT);
    for (int i = 0; i < o; i++)
    {
        if(students[i].goin!=1)
        {
            printf("%s%d\t%s",LBLUE,i+1,DEFAULT);
            kick=1;
        }
    }
    if(kick==0)
    {
        printf("%sNULL%s",LRED,DEFAULT);
    }
    printf("\n\n");
    kick=0;
    printf("%sStudents entered in college for offline semester%s\n",BBLUE,DEFAULT);

    printf("%s -->Students having positive antibody in round 1 :%s\n",BYELLOW,DEFAULT);
    for (int i = 0; i < o; i++)
    {
        if(students[i].chance==0)
        {
            printf("%s%d\t%s",LYELLOW,i+1,DEFAULT);
            kick=1;
        }
    }
    if(kick==0)
    {
        printf("%sNULL%s",LRED,DEFAULT);
    }
    printf("\n");
    kick=0;
    printf("%s -->Students having positive antibody in round 2 :%s\n",BYELLOW,DEFAULT);
    for (int i = 0; i < o; i++)
    {
        if(students[i].chance==1)
        {
            printf("%s%d\t%s",LYELLOW,i+1,DEFAULT);
            kick=1;
        }
    }
    if(kick==0)
    {
        printf("%sNULL%s",LRED,DEFAULT);
    }
    printf("\n");
    kick=0;
    printf("%s -->Students having positive antibody in round 3 :%s\n",BYELLOW,DEFAULT);
    for (int i = 0; i < o; i++)
    {
        if(students[i].chance==2)
        {
            printf("%s%d\t%s",LYELLOW,i+1,DEFAULT);
            kick=1;
        }
    }
    if(kick==0)
    {
        printf("%sNULL%s",LRED,DEFAULT);
    }
    printf("\n");
    printf("%s------------------------------%s\n",BGREEN,DEFAULT);
    printf("%sPHEW !!! %s\n",BGREEN,DEFAULT);
    printf("%sMY INTERNSHIP SALARY IS SAFE.%s\n",BGREEN,DEFAULT);
    printf("%s=================================================================%s\n\n",DEFAULT,DEFAULT);

}

void manufacture_vaccine(Company *company)
{
    while(1)
    {
        if(company -> batch_left == 0)
        {
            break;
        }
        else pthread_cond_wait(&(company->cond_var_zone), &(company->company_mutex));
    }
    printf("%sAll the vaccines prepared by Pharmaceutical Company %d are emptied. Resuming production now.%s\n",BBLUE,company->id,DEFAULT);
    pthread_mutex_unlock(&(company -> company_mutex));
}

void slot_check(Zone *zone)
{
    while(1)
    {
        pthread_mutex_lock(&(waiting_student));
        if(numb_waiting == 0)
        {
            printf("%sNo student is waiting at Zone %d slots, all slots are emptied %s\n",LGREEN,zones->id,DEFAULT);
            pthread_mutex_unlock(&(waiting_student));
            // sleep(2);
            break;
        }
        pthread_mutex_unlock(&(waiting_student));

        if(zone -> slots == zone -> occupancy)
        {
            break;
        }
        else pthread_cond_wait(&(zone-> cond_var_student), &(zone-> zone_mutex));
    }
    pthread_mutex_unlock(&(zone -> zone_mutex));
}

void slot_allotment(int id)
{
    int got_zone = 1;
    while(got_zone==1)
    {
        for(int i=0; i < m; i++)
        {
            pthread_mutex_lock(&(zones[i].zone_mutex));
            if(zones[i].slots - zones[i].occupancy > 0)
            {
                zones[i].occupancy++;
                got_zone = 0;
                students[i].zone_id=zones[i].id;
                // sleep(2);
                printf("%sStudent %d assigned a slot on the Vaccination Zone %d and waiting to be vaccinated%s\n",BCYAN,id,i+1,DEFAULT);
                student_in_slot(i, id);
                break;
            }
            pthread_cond_signal(&(zones[i].cond_var_student));
            pthread_mutex_unlock(&(zones[i].zone_mutex));
        }
    }

    return;
}

void student_in_slot(int i, int id)
{
    pthread_mutex_lock(&(waiting_student));
    numb_waiting--;
    pthread_mutex_unlock(&(waiting_student));
    // sleep(2);
    printf("%sStudent %d on Vaccination Zone %d has been vaccinated which has success probability %Lf %s\n",BRED,id,i+1,zones[i].prob, DEFAULT);
    // sleep(2);
    if(get_prob(zones[i].prob)==1){
        printf("%sStudent %d has tested positive for antibodies.%s\n",BGREEN,id,DEFAULT);
        students[id-1].goin=1;
        printf("%sStuedent %d entered in college for offline semester%s\n",LMAGENTA,id,DEFAULT);
    }
    else
    {
        students[id-1].chance++;
        printf("%sStudent %d has tested negative for antibodies.%s\n",BRED,id,DEFAULT);
    }
    
    pthread_cond_signal(&(zones[i].cond_var_student));
    pthread_mutex_unlock(&(zones[i].zone_mutex));
    return;
}

void * company_thread(void* args)
{
	Company * company = (Company*)args;
    while(1)
    {
        int r = get_random(1,5);
        printf("%sPharmaceutical Company %d is preparing %d batches of vaccines which have success probability %Lf%s\n",LBLUE, company->id,r, company->prob,DEFAULT);
        int w = get_random(2,5);
        sleep(w);
        pthread_mutex_lock(&(company -> company_mutex));
        company->batch_left = r;
        company->capacity = get_random(10,20);
        printf("%sPharmaceutical Company %d has prepared %d batches of vaccines which have success probability %Lf. Waiting for all the vaccines to be used to resume production%s\n",BBLUE, company->id,company->batch_left, company->prob,DEFAULT);        
        manufacture_vaccine(company);
    }
    return NULL;
}

void * zone_thread(void* args)
{
    Zone * zone = (Zone*)args;
    while(1)
    {
        int fluff = 0;
        for(int i=0; i < n; i++)
        {
            pthread_mutex_lock(&(companys[i].company_mutex));
            if(companys[i].batch_left > 0)
            {
                fluff = 1;
                zone -> capacity = companys[i].capacity;
                zone ->prob = companys[i].prob;
                companys[i].batch_left--;
                printf("%sPharmaceutical Company %d is delivering a vaccine batch to Vaccination Zone %d which has success probability %Lf%s\n",LYELLOW,companys[i].id,zone->id,companys[i].prob, DEFAULT);
                printf("%sPharmaceutical Company %d has delivered vaccines to Vaccination zone %d, resuming vaccinations now%s\n",LYELLOW,companys[i].id,zone->id,DEFAULT);
                printf("%sVaccination Zone %d entering Vaccination Phase %s\n",LYELLOW,zone->id,DEFAULT);
                pthread_cond_signal(&(companys[i].cond_var_zone));
                pthread_mutex_unlock(&(companys[i].company_mutex));
                break;
            }
            pthread_cond_signal(&(companys[i].cond_var_zone));
            pthread_mutex_unlock(&(companys[i].company_mutex));
        }
        while(fluff)
        {
            pthread_mutex_lock(&(zone->zone_mutex));
            if(zone->capacity == 0)
            {
                printf("%sVaccination Zone %d has run out of vaccines%s\n",BYELLOW,zone->id,DEFAULT);
                pthread_mutex_unlock(&(zone->zone_mutex));
                break;
            }
            pthread_mutex_lock(&waiting_student);
            zone -> slots = get_random(1,min(8,min(zone->capacity,numb_waiting)));
            pthread_mutex_unlock(&waiting_student);

            zone -> occupancy = 0;
            zone -> capacity -= zone->slots;
            
                printf("%sVaccination Zone %d is ready to vaccinate with %d slots%s\n",BYELLOW,zone->id,zone->slots,DEFAULT);
                sleep(2);
                slot_check(zone);
            
            
        }
    }
    return NULL;
}

void * student_thread(void* args)
{
    Student * student = (Student*)args;
    int arrival_time = get_random(0,20);
    sleep(arrival_time);
    pthread_mutex_lock(&(waiting_student));
    numb_waiting++;
    pthread_mutex_unlock(&(waiting_student));
    pthread_mutex_lock(&(student->student_mutex));
    while(1)
    {
        if(student->goin==1)
        {
            break;
        }
        if(student->chance >2)
        {
            printf("%sStuedent %d sent home for another online semester%s\n",LMAGENTA,student->id,DEFAULT);
            break;
        }
        else{
            printf("%sStudent %d has arrived for his/her %s round of Vaccination%s\n",LCYAN,student->id,round_arr[student->chance],DEFAULT);
            printf("%sStudent %d is waiting to be allocated a slot on a Vaccination Zone%s\n",LCYAN,student->id,DEFAULT);
        }
        slot_allotment(student->id);
    }
    pthread_mutex_unlock(&(student->student_mutex));
    return NULL;
}