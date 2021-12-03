#include "types.h"
#include "stat.h"
#include "user.h"
#include "processInfo.h"
#include "fcntl.h"

int rand[] = {9,1,4,15,17, 14, 12, 11, 9, 5, 1, 3, 2, 16, 7, 8, 12, 17, 8, 19, 20, 8, 17, 1, 4, 12};

// CPU bound
void
random_computation()
{
    double x = 0;
    for (double y = 0; y < 1000000; y += 0.1)
    {
        x = x + (6.9*4.20);
    }
}
void random_io(){
    for(int i=0;i<200;i++){
        printf(1,".");
    }
    sleep(10);
}

int main(int argc, char *argv[])
{
    int n;
    if (argc < 2)
    {
       n=5;
    }
    else n = atoi(argv[1]);
    int burst_time=0;
    int pids[n], returnOrder[n], mapping[500];

    printf(1, "\nTest: Pseudo-Random burst times with CPU bound processes for hybrid scheduler \n");
    for (int i = 0; i < n; i++)
    {
        int id = fork(); //fork a new child process
        if (id < 0)      // Fork error
        {
            printf(1, "Error in Forking \n");
            exit();
        }
        else if (id == 0) // If id==0 child process is created succesfully
        {
            burst_time = rand[i];       // Pick burst time from random array
            set_burst_time(burst_time); // Set the burst time for child process
            random_computation();
            // random_io();
            exit();
        }
        else
        {
            pids[i] = id;    // Store the pid
            mapping[id] = i; // Store the mapping
        }
    }

    for (int i = 0; i < n; i++)
    {
        returnOrder[i] = wait(); // Store the return order
    }

    printf(1, "\nChildren : \n");
    printf(1, "Child#\t\tPID\t\t Burst Time     \n");
    for (int i = 0; i < n; i++)
    {
        printf(1, "%d\t\t %d\t\t %d  \n", i, pids[i], rand[i]); //Print pids and burst times
    }
    printf(1, "\nOrder of completion : \n");
    printf(1, "PID\t\t Burst Time     \n");
    for (int i = 0; i < n; i++)
    {
        printf(1, "%d\t\t %d  \n", returnOrder[i], rand[mapping[returnOrder[i]]]); // Print the return order and corresponding burst times
    }

    exit();
}
