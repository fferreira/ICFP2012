#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

int main(int argc, char * argv[])
{
  signal(SIGINT, SIG_IGN);

  if(argc == 2)
    system(argv[1]);
  else
    printf("Something is wrong here!\n");

  return 0;
}
