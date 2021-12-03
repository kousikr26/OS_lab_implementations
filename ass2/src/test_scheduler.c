#include "types.h"
#include "stat.h"
#include "user.h"
#include "processInfo.h"
#include "fcntl.h"

int rand[] = {9,1,4,15,20, 14, 12, 11, 9, 5, 1, 3, 2, 16, 7, 8, 12, 17, 8, 19, 20, 8, 17, 1, 4, 12};
int inc[] = {1,5,8,10,13,15,16, 17, 18,19,20};
int dec[] = {17,14,12,11,9,8,7,6,5,4,3,2,1};
// CPU bound
void random_computation()
{
    //Dummy function to perform computations that occupy cpu time
    double x = 0;
    for (double y = 0; y < 1000000; y += 1)
    {
        x = x + (6.9*4.20);
    }
}
void random_io(){
    // Dummy function that performs I/O to occupy cpu time
    for(int i=0;i<200;i++){
        printf(1,".");
    }
    sleep(10);
}

int main(int argc, char *argv[])
{
    int n;
    if (argc < 2) // If no argument given take n as 5
    {
       n=5;
    }
    else n = atoi(argv[1]);
    int burst_time=0; 
    //pids stores the pid of processes in order they were created
    //returnOrder stores the completion order of forked processes
    // mapping gives child to pid mapping
    int pids[n], returnOrder[n], mapping[500]; 
    

    
    printf(1, "\nTest 1: Pseudo-Random burst times with CPU bound processes\n");
    
    for (int i = 0; i < n; i++)
    {
        int id = fork(); //fork a new child process
        if (id < 0) // Fork error
        {
            printf(1, "Error in Forking \n");
            exit();
        }
        else if (id == 0) // If id==0 child process is created succesfully 
        {
            burst_time = rand[i]; // Pick burst time from random array
            set_burst_time(burst_time); // Set the burst time for child process
            random_computation();
            // random_io();
            exit();
        }
        else
        {
            pids[i] = id; // Store the pid
            mapping[id]=i; // Store the mapping
        }
    }

    for (int i = 0; i < n; i++)
    {
        returnOrder[i] = wait(); // Store the return order
    }

    printf(1, "\nChildren : \n"); 
    printf(1, "Child#\t\tPID\t\t Burst Time     \n");
    for (int i = 0; i < n; i++){
        printf(1, "%d\t\t %d\t\t %d  \n", i, pids[i], rand[i]); //Print pids and burst times
    }
    printf(1, "\nOrder of completion : \n");
    printf(1, "PID\t\t Burst Time     \n");
    for (int i = 0; i < n; i++){
        printf(1, "%d\t\t %d  \n", returnOrder[i], rand[mapping[returnOrder[i]]]); // Print the return order and corresponding burst times
        
    }
    //---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


    printf(1, "\nTest 2: Pseudo-Random burst times with I/O bound processes\n");
    for (int i = 0; i < n; i++)
    {
        int id = fork();
        if (id < 0)
        {
            printf(1, "Error in Forking \n");
            exit();
        }
        else if (id == 0)
        {
            burst_time = rand[i];
            set_burst_time(burst_time);
            // random_computation();
            random_io(); //Use I/o bound processes
            exit();
        }
        else
        {
            pids[i] = id;
            mapping[id] = i;
        }
    }

    for (int i = 0; i < n; i++)
    {
        returnOrder[i] = wait();
    }

    printf(1, "\nChildren : \n");
    printf(1, "Child#\t\tPID\t\t Burst Time     \n");
    for (int i = 0; i < n; i++)
    {
        printf(1, "%d\t\t %d\t\t %d  \n", i, pids[i], rand[i]);
    }
    printf(1, "\nOrder of completion : \n");
    printf(1, "PID\t\t Burst Time     \n");
    for (int i = 0; i < n; i++)
    {
        printf(1, "%d\t\t %d  \n", returnOrder[i], rand[mapping[returnOrder[i]]]);
    }

    //---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    printf(1, "\nTest 3: Increasing burst times with both CPU and I/O bound processes\n");
    for (int i = 0; i < n; i++)
    {
        int id = fork();
        if (id < 0)
        {
            printf(1, "Error in Forking \n");
            exit();
        }
        else if (id == 0)
        {
            burst_time = inc[i];
            set_burst_time(burst_time);
            random_computation();
            random_io();
            exit();
        }
        else
        {
            pids[i] = id;
            mapping[id] = i;
        }
    }

    for (int i = 0; i < n; i++)
    {
        returnOrder[i] = wait();
    }

    printf(1, "\nChildren : \n");
    printf(1, "Child#\t\tPID\t\t Burst Time     \n");
    for (int i = 0; i < n; i++)
    {
        printf(1, "%d\t\t %d\t\t %d  \n", i, pids[i], inc[i]);
    }
    printf(1, "\nOrder of completion : \n");
    printf(1, "PID\t\t Burst Time     \n");
    for (int i = 0; i < n; i++)
    {
        printf(1, "%d\t\t %d  \n", returnOrder[i], inc[mapping[returnOrder[i]]]);
    }

    //---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


    printf(1, "\nTest 4: Decreasing burst times with both CPU and I/O bound processes\n");
    for (int i = 0; i < n; i++)
    {
        int id = fork();
        if (id < 0)
        {
            printf(1, "Error in Forking \n");
            exit();
        }
        else if (id == 0)
        {
            burst_time = dec[i];
            set_burst_time(burst_time);
            random_computation();
            random_io();
            exit();
        }
        else
        {
            pids[i] = id;
            mapping[id] = i;
        }
    }

    for (int i = 0; i < n; i++)
    {
        returnOrder[i] = wait();
    }

    printf(1, "\nChildren : \n");
    printf(1, "Child#\t\tPID\t\t Burst Time     \n");
    for (int i = 0; i < n; i++)
    {
        printf(1, "%d\t\t %d\t\t %d  \n", i, pids[i], dec[i]);
    }
    printf(1, "\nOrder of completion : \n");
    printf(1, "PID\t\t Burst Time     \n");
    for (int i = 0; i < n; i++)
    {
        printf(1, "%d\t\t %d  \n", returnOrder[i], dec[mapping[returnOrder[i]]]);
    }

    //---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    exit();
}
