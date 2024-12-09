/* hello_signal.c */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>




int signal_received = 0;

void handler(int signum)
{ //signal handler
  printf("Hello World!\n");
  signal_received = 1;
  alarm(5);
}

int main(int argc, char * argv[])
{
  signal(SIGALRM,handler); //register handler to handle SIGALRM
  alarm(5); //Schedule a SIGALRM for 5 seconds
  while (1){
    if (signal_received){
     printf("Turning was right!\n");
     signal_received =0; 
  }
  pause();
  }
  
  return 0;
}


