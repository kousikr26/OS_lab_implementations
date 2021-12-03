#include "param.h"
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"
#include "syscall.h"
#include "traps.h"
#include "memlayout.h"
void * ptr_arr[10];
void fill ( void * ptr){
	/* Function to fill the malloced memory with some characters */

	char *charptr;
	charptr = (char *)ptr;
	for (int j = 0; j < 4096; j++)
	{
		charptr[j] = 'a';
	}
}
int validate(void * ptr){
	/* Function to validate the malloced memory  */
	char *charptr;
	charptr = (char *)ptr;
	for (int j = 0; j < 4096; j++)
	{
		if (charptr[j] != 'a')
		{
			printf(1, "Memory Invalidation!\n\n");
			return 0;
		}
	}
	return 1;
}

void child()
{
	/*
		Child process which allocates memory with malloc 
	*/

	void *ptr; // pointer to malloced memmory
	
	
	int i = 0;
	while (i < 10) // While loop for 10 iterations
	{
		ptr = malloc(4096); // Malloc 4KB
		if (ptr == 0){
			printf(1, "Memory allocation failure!\n\n");
			exit();
		}
		fill(ptr);
		ptr_arr[i]=ptr;
		
		i++;
	}

	
	for(int i=0;i<10;i++){ // Validate each malloced memory
		if(validate(ptr_arr[i])==0) exit();
		// printf(1, ".");
	}
	
	exit();
	
}

int main(int argc, char *argv[])
{
	int i, pid, pids[20];
	

	printf(1,"Creating child processes with PID's : \n");
	for (i = 0; i < 20; i++)
	{
		pid = fork();
		if (pid == 0)
		{
			child(i);
			sleep(5);
			exit();
		}
		else if(pid>0){

			pids[i] = pid;
			printf(1,"%d, ",pids[i]);
		}
		else{
			printf(1,"Error in forking \n");
			exit();
		}
		
	}
	printf(1,"\n\n");

	for(i=0;i<20;i++){
		int x=wait();
		printf(1,"Validated child process %d\n",x);
	}
	
	printf(1, "------------------------------------------------------------------------\n");
	printf(1, "Test completed successfully!\n");
	exit();
}

