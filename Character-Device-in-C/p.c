#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include <string.h>

#define NUM_LOOPS	15 /* number of loops to perform. */
#define SIZE_OF_STRING 13



void delay_fun(){

	  int random_number = rand();
	  //printf("%d\n",random_number);
	  int N = 10;
	  int rand_capped = random_number % N;  //between 0 and 32
	 //printf("delay_fun : sleeping for %d seconds\n",rand_capped);
	  sleep(rand_capped);
}

int main(int argc, char *argv[]){
		
		if(argc != 2)
		{
			printf("\n Invalid arguments: run as ./p <no of producers> \n");
			exit(1);
		}

		srand (time(NULL));
		int pid;

		int num_process = atoi(argv[1]);
		int p_number;
		char buf[SIZE_OF_STRING];
		int i;

		//printf("main parent process id : %d\n",  getpid());

		int fd_p;

		fd_p = open("/dev/mypipe", O_WRONLY);
		if(fd_p < 0)
		{
			fprintf(stderr, "error opening file \n"); 
				 exit(1); 
		}

		for(p_number = 0; p_number < num_process; p_number++) {
		   
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
			// child specific stuff. p_number is set to the "child number
			for (i=0; i<NUM_LOOPS; i++) {
				   
				   // delay_fun();
				   // sprintf(buf, "Producer %d generated this string %d", p_number,i);
				   
				   if(i<=9)
						sprintf(buf, "Producer %d  %d", p_number,i);
					else
						sprintf(buf, "Producer %d %d", p_number,i);
					
					//printf("size of buffer %zu\n", sizeof buf);
				   // printf("len %zu\n", strlen(buf) );
					
					int n = write( fd_p, buf, strlen(buf));
						//don't forget to check for write error. 
					if(n == -1){
						printf("Write error	!\n");
						exit(1);
					}
				
				printf(" producer: '%d' wrote successfully :  %s\n", p_number,buf); 
				
				//printf("%s\n",buf);
				
				fflush(stdout);
			}
		}
		else {
			//printf("**** int else **** parent process id : %d\n",  getpid());
			//parent stuff
			}
		close(fd_p);
}
