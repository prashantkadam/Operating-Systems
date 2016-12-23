#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include <string.h>

#define NUM_LOOPS	15	 /* number of loops to perform. */
#define SIZE_OF_STRING 13



void delay_fun(){

	  int random_number = rand();
	  //printf("%d\n",random_number);
	  int N = 10;
	  int rand_capped = random_number % N;  //between 0 and 32
	  //printf("delay_fun : sleeping for %d seconds\n",rand_capped);

	  sleep(rand_capped);
}


int main(int argc, char *argv[])
{
		
		if(argc != 2)
		{
			printf("\n Invalid arguments: run as ./p <no of consumer> \n");
			exit(1);
		}
		srand (time(NULL));
		int pid;
		int num_consumer_process = atoi(argv[1]);
		int procNum;
		char buf[SIZE_OF_STRING];
		
		int i,j;
		
		for(j = 0;j<SIZE_OF_STRING;j++)
		{
			buf[j] = '\0';
		}

		
	int fd_c;
	fd_c = open("/dev/mypipe", O_RDONLY);
	if( fd_c == -1)
	{
		fprintf(stderr, "error opening file \n"); 
		 exit(1); 
	}

		

		for(procNum = 0; procNum < num_consumer_process; procNum++) {
			
			pid = fork();
			
			 if (pid == -1) { 
				 fprintf(stderr, "fork failed\n"); 
				 exit(1); 
			 }
	 
			if (pid == 0) {
				break;
			}
		}

		if (pid == 0) {
			
			for (i=0; i<NUM_LOOPS; i++) {
			//sleep(10);
			//delay_fun();
			int n = read( fd_c,buf,SIZE_OF_STRING);		
				//don't forget to check for read error
			if(n == -1){
				printf("read error!\n");
				exit(0);
			} 
			printf(" consumer:%d reads : %s\n",procNum, buf);
			//printf("%s\n",buf);			
			fflush(stdout);
			}
		}
		else {
			//parent stuff
		}
		close(fd_c);
}
