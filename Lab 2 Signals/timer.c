/* hello_signal.c */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>




int signal_received = 0;
int alarm_count =0;
time_t start_time;

void handler(int signum)
{ //signal handler
  printf("Hello World!\n");
  signal_received = 1;
  alarm_count++;
  alarm(5);
}

void signal_int_handler(int signum){
  time_t end_time;
  time(&end_time);

  printf("Total Time %ld seconds\n",end_time-start_time);
  printf("Number of alarms %d \n",alarm_count);
  exit(0);

}

int main(int argc, char * argv[])
{
  time(&start_time);
  signal(SIGALRM,handler);
  signal(SIGINT, signal_int_handler); //register handler to handle SIGALRM
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


