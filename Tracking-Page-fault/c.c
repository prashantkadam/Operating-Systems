#include <stdio.h>
#include <time.h>
#include <sys/wait.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define NUM_LOOPS	1
#define SIZE_OF_STRING 100

#include "info.h"


int main(int argc, char *argv[])
{
		
		srand (time(NULL));
		int pid;
		int num_consumer_process = 1;
		int procNum;

		struct information{
			unsigned long address_array;
			struct timespec timenow;
			};

		struct information buf[1000];
		int i,j;
		
		

		
	int rc,fd_c;
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

			printf("%d\n", _CASE_NUMBER);

			  rc = ioctl(fd_c, _CASE_NUMBER, &buf);
			  printf("ioctl(fd, _CASE_NUMBER, &test1) = %d\n", rc);


			
			if(rc == -1){
				printf("read error!\n");
				exit(0);
			} 
			 
			FILE *fp;
		   fp = fopen("file.dat", "w+");
		   if(fp == NULL){
		   	printf("%s\n", "Error");
		   	return -1;
		   }
			int t = 0;
			for(t = 0;t<1000;t++){
				 printf("address :- 0x%lx \n", buf[t].address_array);
				 printf("time %lld.%.9ld \n", (long long)buf[t].timenow.tv_sec, buf[t].timenow.tv_nsec);
				 fprintf(fp, "%ld	", buf[t].timenow.tv_sec * 1000000000 + buf[t].timenow.tv_nsec);
				 fprintf(fp,"%ld\n", buf[t].address_array);
			}			
			fflush(stdout);
			fclose(fp);
			}
		}
		else {
		}
		close(fd_c);
}
