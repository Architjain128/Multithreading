# **Question 1**
# How to run
- Execute following commands in termainl in `q2` folder
     ```
     -> gcc q1.c -lpthread
     -> ./a.out
     ```
 - Follow the instructions as per output statements from the runing the program.
# Code breakdown
### Modified Merge Sort 
    
+ Using selection sort for **array length < 5**
    ```
    void selectionSort(int arr[], int s,int e)  
    {  
        int i, j, mini;  
        int n = e-s+1 ;
        f(s,n-1,i)  
        {  
            mini = i;  
            f(i+1,n,j)  
            if (arr[j] < arr[mini])  
                mini = j;  
            swap(&arr[mini], &arr[i]);  
        }  
    } 
    ```
+ Merging small sorted arrays 
    ```
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
    ```
+ Those 2 functions mentioned above are to be called in this `mergeSelect` function which itself to be called in main function to implement modified Merge sort 
    ```
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
    ```
+ These functions are also been used in **Concurrent** as well as **Threaded** procedures 

### Concurrent Version
+ This part of the code checks for array length and implmenets selection sort if length is less than or equal to 5. If it is greater, it partitions the array into left and right part and, creates a process by forking to sort left part. While in the parent, it will forks again to create a process to sort right part. In the parent, it waits for the child which processes sorting to left and right part, for a merge. (As instructed in question)
+ Setting up `shm_array` as shared array which will be shared among all the childs created at every level of concurrent mergesort (modified)
    ```
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
    ```
+ Using `selectionSort`and `mergeSelect` funtion mentioned earlier and mereging by `merge` function
    ```
    void con_mergeSelect(int arr[], int l, int r) 
    { 
        if (l < r) 
        { 
            int m = l + (r - l) / 2; 
            if (r-1< 4)  
            { 
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
    ```


### Threaded Version
+ Threaded implementation almost remains same for small arrays having length less than 5, but for larger arrays they are divided and each passed in new thread for `mergeSelect_thread` function with the help of `struct arg` 
    ```
    struct arg
    {
        int* arr;  
        int l, r;  
    };
    ```

    ```
    void *mergeSelect_thread(void *a) 
    { 
        struct arg *temp = (struct arg*) a;
        int *arr = temp->arr;
        int l=temp->l, r=temp->r;
        if (l < r) { 
            int m = l + (r - l) / 2; 
            if (r-1< 4) 
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
    ```

# Observation
* For sample inputs I choose array of length n having random values chossen as in limit of integer
* _**Since runtime depends on the pc specifiactions so here if just a rough ratio as x:1 between**_
    + concurrent mergesort and sequential mergesort
        - For n = 10   : 170.3848037664
        - For n = 100  : 18.19921611194
        - For n = 1000 : 2.234959279829

    + threaded mergesort and sequential mergesort
        - For n = 10   : 327.7733001312
        - For n = 100  : 358.1100657668
        - For n = 1000 : 87.32188665854

# Conclusion

## Concurrent Version
In distributed memory systems, data movement between processors can cause problems. The cost of moving data between processors can possibly become a greater factor than the execution time. Due to this, the task of moving data between processors should be kept to the minimum.
## Threaded Version
A phenomenon that can occur is parallel slowdown. In this, the parallelisation of an algorithm makes the execution of an application to run slower. This is most often related to the communication between processing threads. After a certain number threads have been created, in relation to the purpose of the application, threads will spend most of its time communicating with each other rather than processing data. With the creation of each processing thread, comes a cost in terms of an overhead. When the total cost of the overhead becomes a greater factor than the extra resources it provides, the parallel slowdown occurs.

## Summing up
Sequential Modified Mergesort runs faster than concurrent and threaded mergesort due to no overhead of data structure needed to create new threads and seprate stacks for each concuurent process 

+ Time of sequential modified Mergesort  <  Time of concurrent Mergesort
+ Time of sequential modified Mergesort  <  Time of multithreaded Mergesort




# BONUS
As you can see i also implemented threaded modified mergesort 😎 