# **Question 2**
# How to run
- Execute following commands in termainl in `q2` folder
     ```
     -> gcc q2.c -lpthread
     -> ./a.out
     ```
 - Follow the instructions as per output statements from the runing the program.
 - **Max values of inputs can be less than 1000**

# Code Break down
## Structs
have a struct representing each component where components are Pharmaceutical Company as `Company`, Vaccination Zones as `Zone` and Students arrives to college as `Student` respectively.
```
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
```
+ `id` represents id of a Pharmaceutical Company
+ `batch_left` represents number of batches left of a vaccine produced by a Pharmaceutical Company
+ `capacity` represents number of vaccine in each batch of a Pharmaceutical Company
+ `prob` represents success probability of vaccine manufactured by a Pharmaceutical Company
+ `company_mutex` represents mutex used per Pharmaceutical Company
+ `company_thread_id` represents thread used per Pharmaceutical Company
+ `cond_var_zone` represents conditional variable used for zone in which vaccine will be dilivered
```
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
```
+ `id` represents id of a Vaccination Zone
+ `capacity` represents number of vaccine in each batch of a Vaccination Zone
+ `slots` represents number of vaccination slots in each Vaccination Zone
+ `prob` represents success probability of vaccine used by a Vaccination Zone
+ `zone_mutex` represents mutex used per Vaccination Zone
+ `zone_thread_id` represents thread used per Vaccination Zone
+ `cond_var_student` represents conditional variable used for student which is to be vaccinated
```
struct Student
{
    int id;
    int zone_id;
    int chance;
    int goin;
    pthread_t student_thread_id;
    pthread_mutex_t student_mutex;

};
```
+ `id` represents id of a Student
+ `zone_id` represents id of zone which is alloted to each Student
+ `chance` represents round of vaccination for each Student
+ `goin` represents a flag for successfull positive antibody test for each Student
+ `student_mutex` represents mutex used per Student
+ `student_thread_id` represents thread used per Student
## Other variables
+ Globally defined variables
    - `n` number of Pharmaceutical Companies
    - `m` number of Vaccination Zones
    - `o` number of Students arrived to college
    - `numb_waiting` number of students waiting at a time;
+ Locally defined array
    - `p` stores probability values of each company 
+ Globally defined structs
    - `Company companys[1000] `
    - `Zone zones[1000]`
    - `Student students[1000]`

## Functions
+ For returning any random value in between `a,b` both inclusive
    ```
    int get_random(int a,int b)
    {
        if(a==b || b==0)
        {
            return a;
        }
        int r=a+(rand()%(b-a+1));
        return r;
    }
    ```
+ For returning 1 with probability `a` else 0 

    ```
    int get_prob(double a)
    {
        double r = (double)rand() / RAND_MAX;
        if(r<=a)
        return 1;
        return 0;
    }
    ```
+ For returning min of `a,b` 

    ```
    int min(int a, int b)
    {
        if(a<b)
        return a;
        return b;
    }
    ```
* To generate a short report after simulation
    ```
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
    ```
+ To start manufacturing vaccines when all batches are delivered (here busy waiting is avoided by the use of conditional variable) 
    ```
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
    ```
+ To check all the slots of a zone are filled or not 
    ```
    void slot_check(Zone *zone)
    {
        while(1)
        {
            pthread_mutex_lock(&(waiting_student));

            if(numb_waiting == 0)
            {
                printf("%sNo student is waiting at Zone %d slots, all slots are emptied %s\n",LGREEN,zones->id,DEFAULT);
                pthread_mutex_unlock(&(waiting_student));
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
    ```
+ In tihs function we iterate over all zone and the first zone with empty slots if chossen and that zone is assigned to the student and then called `student_in_slot` function
    ```
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
                    sleep(2);
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
    ```
+ In this function each student is vaccinated and checked for antibody test if positive setting `goin = 1` else increasing `chance++` in struct of student with `id`
    ```
    void student_in_slot(int i, int id)
    {
        pthread_mutex_lock(&(waiting_student));
        numb_waiting--;
        pthread_mutex_unlock(&(waiting_student));
        sleep(2);
        printf("%sStudent %d on Vaccination Zone %d has been vaccinated which has success probability %Lf %s\n",BRED,id,i+1,zones[i].prob, DEFAULT);
        sleep(2);
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
    ```
## Threaded Functons
+ Manufacture vaccine with random capacity and in randon manufacturing time and then after vaccine is maufacture it calls `manufacture_vaccine(company)`
    ```
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
    ```
+ Generates random slots of vaccination for each zone and i use this function to assigned values to zone struct from company's struct such as probability and capacity and updating remaining vaccine in a zone
    ```
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
    ```
+ Assign random arrival time to each student and keep them vaccination till either they gets there antibody test positive or they are vaccinted 3 times
    ```
    void * student_thread(void* args)
    {
        Student * student = (Student*)args;
        int arrival_time = grt_random(0,20);
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
    ```
## Main Function
Taking inputs and creating mutex and threads for each company, zone and student then after completion of all threaded process all threads are joined and mutex are been destroyed and at last a report is generated by `report` function
```
    int main()
{
    srand(time(0));
    printf("Enter number of pharmaceutical company : "); 
    scanf("%lld",&n);
    printf("Enter number of vaccination zones : "); 
    scanf("%lld",&m);
    printf("Enter number of students : "); 
    scanf("%lld",&o);
    ld p[n];
    printf("Enter probabilities of success rate of all companies : \n");
    f(0,n,i) scanf("%Lf",&p[i]);

    if(n<=0 || m<=0 || o<=0)
    {
        printf("%sInvalid input : Input must be greater than `0`%s\n",BRED,DEFAULT);
        return 0;
    }
    if(n>=1000 || m>=1000 || o>=1000)
    {
        printf("%sInvalid input : Input must be less than `1000`%s\n",BRED,DEFAULT);
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
```
# Logic behind all this 😕
+ All inputs are saved in respective structs and to start the simulation i have created one thread for each company, zone and students.
+ thread for a company calls `comany_thread` which starts the manufacturing of vaccine by calling `manufacture_vaccine(Company *company)` which manufacture vaccine if no batch is left and waits for a signal from zone to supply it.
+ thread for student assigns needed randaom variables and sleep till there arrival time then printing that they have arrived and waiting for slot allotment and call `slot_allotment` function which allots slots in a zone to a student and then which again calls a function `student_in_slot` which vaccinate the student and check antibody test and decide to send a student home or not and increase chance variable of vaccinated student and after that signals a zone to decrease its vaccination count by 1 and release student from slot vaccination
+ thread for zone calls `zone_thread` which decides number of slot for vaccination that can range in  1 to minimum of 8,number of vaccine left and number of students waiting and then wait for student's conditional variable signal to start vaccination if `vaccine_left > 0` else it signals the company to manufacturing new batch of vaccinace.
+ all probabilities are stored in struct of company as well as zone using a company's vaccine so that they can be used at time of vaccination with a funtion `get_prob` if returns 1 then student is antibody test is positve, else it will go through procedure again
till it chance become greater than 3 and then sent back to home for anoother online semester.

* _**all funtions are desribed discriptively above in `code breakdown section`**_
