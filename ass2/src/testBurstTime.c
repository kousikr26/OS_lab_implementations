#include "types.h"
#include "stat.h"
#include "user.h"

int main()
{
  printf(1,"For PID = %d \n",getpid());
  printf(1, "Burst Time Before: %d \n", get_burst_time()); // Get burst time of current process
  set_burst_time(10); // Set burst time of current process
  printf(1 , "Burst Time After: %d \n" , get_burst_time());
  exit();
}