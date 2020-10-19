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

struct arg
{
    int* arr;  
    int l, r;  
};
void swap(int *xp, int *yp)  
{  
    int temp = *xp;  
    *xp = *yp;  
    *yp = temp;  
}  

void selectionSort(int arr[], int s,int e)  
{  
    int i, j, min_idx;  
    int n = e-s+1 ;
    f(s,e,i)  
    {  
        min_idx = i;  
        f(i+1,e+1,j)  
        if (arr[j] < arr[min_idx])  
            min_idx = j;  
  
        swap(&arr[min_idx], &arr[i]);  
    }  
} 

void merge(int arr[], int l, int m, int r) 
{ 
    int i, j, k; 
    int n1 = m - l + 1; 
    int n2 = r - m; 
  
    int L[n1], R[n2]; 
  
    f(0,n1,i) 
        L[i] = arr[l + i]; 
    f(0,n2,j) 
        R[j] = arr[m + 1 + j]; 
  
    i = 0; 
    j = 0; 
    k = l; 
    while (i < n1 && j < n2) { 
        if (L[i] <= R[j]) { 
            arr[k] = L[i]; 
            i++; 
        } 
        else { 
            arr[k] = R[j]; 
            j++; 
        } 
        k++; 
    } 
  
    while (i < n1) { 
        arr[k] = L[i]; 
        i++; 
        k++; 
    } 
  
    while (j < n2) { 
        arr[k] = R[j]; 
        j++; 
        k++; 
    } 
} 
  
void mergeSelect(int arr[], int l, int r) 
{ 
    if(l<r)
    {
        int m = l + (r - l) / 2; 
        if (r-1< 4) 
        { 
            selectionSort(arr,l,r);
        }
        else
        {
            mergeSelect(arr, l, m); 
            mergeSelect(arr, m + 1, r); 
            merge(arr, l, m, r); 
        }
    }
} 

void con_mergeSelect(int arr[], int l, int r) 
{ 
    if (l < r) { 
         int m = l + (r - l) / 2; 
        if (r-l< 4) { 
            selectionSort(arr,l,r);
        }
        else
        {
  
            pid_t lpid=fork();
            if(lpid<0)
            {
                perror("forking");
                exit(EXIT_FAILURE);
            }
            else if(lpid==0)
            {
                mergeSelect(arr, l, m);
                exit(EXIT_SUCCESS);
            }
            else
            {
                pid_t rpid=fork();
                if(rpid<0)
                {
                    perror("forking");
                    exit(EXIT_FAILURE);
                }
                else if(rpid==0)
                {
                    mergeSelect(arr, m + 1, r); 
                    exit(EXIT_SUCCESS);
                }
                else
                {
                    int st1,st2;
                    waitpid(lpid,&st1,0);
                    waitpid(rpid,&st2,0);
                }
            }
            merge(arr, l, m, r); 
        }
    } 
} 
void *mergeSelect_thread(void *a) 
{ 
    struct arg *temp = (struct arg*) a;
    int *arr = temp->arr;
    int l=temp->l, r=temp->r;
    if (l < r) { 
        int m = l + (r - l) / 2; 
        if (r-l< 4) 
        {
            selectionSort(arr,l,r);
        }
        else
        {
            struct arg a1;
            a1.l = l, a1.r = m, a1.arr = arr; 
            pthread_t l_tid;
            pthread_create(&l_tid, NULL,mergeSelect_thread, &a1);
            pthread_join(l_tid, NULL);
    
            struct arg a2;
            a2.l = m+1, a2.r = r, a2.arr = arr; 
            pthread_t r_tid;
            pthread_create(&r_tid, NULL,mergeSelect_thread, &a2);
            pthread_join(r_tid, NULL);

            merge(arr, l, m, r);
        } 
    } 
} 
  


int main() 
{ 
    ld d2,d3,d4;
    printf("Enter number of variables you want to sort\n");
    int n;
    scanf("%d",&n);
    int shmid; 
    key_t key = IPC_PRIVATE; 
    size_t SHM_SIZE = sizeof(int)*(n+2); 
    int *shm_array; 

     if ((shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666)) < 0) 
    { 
        perror("shmget"); 
        _exit(1); 
    }  
    if ((shm_array = shmat(shmid, NULL, 0)) == (int *) -1) 
    { 
        perror("shmat"); 
        _exit(1); 
    } 

    printf("Enter %d numbers each sepreated by a space \n",n);
    int arr2[n],arr3[n];
    f(0,n,i)
    {
        scanf("%d",&arr2[i]); 
        arr3[i]=arr2[i];
        shm_array[i]=arr2[i];
    }

    struct timespec ts;
    pthread_t ptd;
    struct arg a; 
    a.l = 0;
    a.r = n-1; 
    a.arr = arr3;
    ld t1,t2;
  printf("%s=========================================================================================%s\n",DEFAULT,DEFAULT);


// modified mergesort
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    t1=ts.tv_nsec/(1e9)+ts.tv_sec;
    mergeSelect(arr2, 0, n-1);
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    t2=ts.tv_nsec/(1e9)+ts.tv_sec;
    d2=t2-t1;
    printf("%sSorted array by modified MergeSort :%s\n",BBLUE,DEFAULT);  
    f(0,n,i)printf("%d ",arr2[i]);
    printf("\n%sTime taken by modified MergeSort   : %0.10Lf %s\n",LBLUE,d2,DEFAULT); 


// concurrent mergesort
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    t1=ts.tv_nsec/(1e9)+ts.tv_sec;
    con_mergeSelect(shm_array, 0, n - 1);
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    t2=ts.tv_nsec/(1e9)+ts.tv_sec;
    d3=t2-t1;
    printf("\n%sSorted array by concurrent MergeSort :%s\n",BBLUE,DEFAULT);  
    f(0,n,i)printf("%d ",shm_array[i]);
    printf("\n%sTime taken by concurrent MergeSort : %.10Lf %s\n",LBLUE,d3,DEFAULT); 

// threaded concurrent mergesort
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    t1=ts.tv_nsec/(1e9)+ts.tv_sec;
    pthread_create(&ptd, NULL, mergeSelect_thread, &a);
    pthread_join(ptd, NULL);
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    t2=ts.tv_nsec/(1e9)+ts.tv_sec;
    d4=t2-t1;
    printf("\n%sSorted array by threaded MergeSort :%s\n",BBLUE,DEFAULT);  
    f(0,n,i)printf("%d ",a.arr[i]);
    printf("\n%sTime taken by threaded MergeSort   : %.10Lf%s\n",LBLUE,d4,DEFAULT); 

    printf("%s=========================================================================================%s\n",DEFAULT,DEFAULT);
    printf("%sSUMMARY%s\n",BGREEN,DEFAULT);
    printf("%s------------%s\n",BGREEN,DEFAULT);
    printf("%s    Modified mergesort is :\n",LYELLOW);
    printf("       <+> [%18.10Lf] times faster than concurrent modified MergeSort\n",d3/d2);
    printf("       <+> [%18.10Lf] times faster than threaded modified MergeSort%s\n",d4/d2,DEFAULT);
    printf("%s=========================================================================================%s\n\n",DEFAULT,DEFAULT);
    return 0; 
} 