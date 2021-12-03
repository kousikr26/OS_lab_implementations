#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int main(void){
  ps(); //Call the ps syscall to dump process information
  exit();
}