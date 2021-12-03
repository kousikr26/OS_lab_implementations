#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char *argv[])
{
  char * buf=(char *)malloc(1253*sizeof(char)); //Create a buffer of required size
  printf(1,"syscall returned %d\n",wolfie(buf,1253)); //Perform the syscall 
  printf(1, "%s\n", buf); //print the buf to print wolfie
  free(buf); //Free the buf
  exit();
}
