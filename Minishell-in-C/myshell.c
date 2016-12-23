#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include "helper.c"

#define STOPPED 'S'
#define BACKGROUND 'B'
#define RUNNIG 'R'
#define HOMEPATH "HOME"


int background = 0;
int input_redirect = 0;
int output_redirect = 0;
int piped_command = 0;
int count_piped = 0;

int childId;
int c_pid;

int test = 0;
pid_t master_process; 
/* type definition of "pointer to a function that takes integer argument and returns void */
typedef void Sigfunc(int);

/* This function  installs a signal handler for 'signo' signal */
Sigfunc *install_signal_handler(int signo, Sigfunc *handler)
{
	struct sigaction act, oact;

	act.sa_handler = handler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;

	if( signo == SIGALRM ) {
#ifdef SA_INTERRUPT
		act.sa_flags |= SA_INTERRUPT;  /* SunOS 4.x */
#endif
	} else {
#ifdef SA_RESTART
		act.sa_flags |= SA_RESTART;  /* SVR4, 4.4BSD */
#endif
	}

	if( sigaction(signo, &act, &oact) < 0 )
		return (SIG_ERR);

	return(oact.sa_handler);
}


void STOPProcess(pid_t processId){
	//printf("%s\n", "in STOPProcess method");
	updateJobStatus(processId,STOPPED);
  	kill(processId,SIGSTOP);
}

void killProcess(pid_t processId){
	//printf("%s\n", "in killProcess method");
	kill(processId , SIGKILL);
	remove_pid(processId);

}


void int_handler(int sig)
{
 // printf("!!Got interrupt!!\n");
  int pid = getpid();
  //printf("--------start int_handler-----------%d --- %d--%d\n",c_pid,pid,master_process);
  //printf("%d\n", c_pid );
  if(master_process != pid)
  	killProcess(c_pid);
//  printf("--------end int_handler-----------\n");

  //kill();
  //exit();
}

void stp_handler(int sig)
{
 // signal(sig, SIG_DFL);
  //printf("!!Got  stp interrupt!!\n");
  int pid = getpid();
 // printf("--------start stp_handler-----------\n");
 // printf("%d\n", c_pid);
  updateJobStatus(c_pid,STOPPED);
  kill(c_pid,SIGSTOP);
  printf("\n");
 // printf("--------end stp_handler-----------\n");
  return;
  //exit();
}


void chld_handler(int sig)
{
	//printf("%s\n", "chld_handler");
  pid_t	pp;
  int status;
  int parent = getpid();
  if(is_any_bcgSet() == 1){
 	 pp = wait(&status);
  	 remove_pid(pp);
  	remove_last_isBcg();
  	printf("\n");
  	// test = 0;
  	// printf("in chld_handler pp- %d\n", pp);
  	// printf("Parent continues...\n");
  	 
}
}



void readInput(char *command_){
	//printf("command_  in read input %c\n", command_[0]);
	char c;
	int v = 1 ;
	

	int stringAccepting = 0;

	while((c = getchar())!='\n'){
		

		if(c =='&'){
				background = 1;v--;
			}
		else if(c =='<'){
				input_redirect = 1;v--;
			}
		else if(c =='>'){
				output_redirect = 1;v--;
			}
		else if(c =='|'){
				piped_command = 1;
				*(command_ + v) = c; // not removing pipe | from actual command
					v++;
				count_piped++;
			}
		else{
			*(command_ + v) = c;
			v++;
		}
	}
	command_[v] = '\0';
	stringAccepting = 0;
}

void sendsigcont(int jobid){
	int status;
	int processId = getProcessId(jobid);
	updateJobStatus(processId,RUNNIG);
	kill(processId,SIGCONT);
	install_signal_handler(SIGINT, int_handler);
			do 	{
                    int wpid = waitpid(-1, &status, WUNTRACED | WCONTINUED);
                    if (wpid == -1) 
                    {
                    	break;
                    	perror("---- waitpid waitpid");
                    //exit(EXIT_FAILURE);
                     }

                   if (WIFEXITED(status)) 
                   {
                  //  printf("exited, status=%d\n", WEXITSTATUS(status));
                    remove_pid(processId);
                   }     
                   else if (WIFSIGNALED(status)) 
                   {
                 //   printf("killed by signal %d\n", WTERMSIG(status));
                    remove_pid(processId);
                   } 
                   else if (WIFSTOPPED(status)) 
                   {
                   // printf("process %d stopped by signal %d\n",(int)processId, WSTOPSIG(status));
                   } 
                   else if (WIFCONTINUED(status)) 
                   {
                      // printf("continued\n");
                   }
                } while (!WIFEXITED(status) && !WIFSIGNALED(status) && !WIFSTOPPED(status));
            
        }


void generate_Path_arguments(char **argument,char *command_){
	int v;	
	char *token;
	char *bin;
	char *t = "/bin/";
	int t_len =  strlen(t);
	//printf(" generate_Path_arguments command -> %s\n", command_);
	token = strtok(command_," ");
	int token_len = strlen(token);
	int pathSize = token_len+t_len;
		int i = 0;
	while(token != NULL){
			//printf("%s\n", "in while");		
			argument[i] = token;
			//printf("%s\n", token);
			token = strtok(NULL," ");
			i++;

		}
	argument[i] = NULL;
	//printf("---------------------------------------------------%s\n", path);
}

void myshellpromt(){
	printf("%s", "minish :~>");
}


void execute(char **argument){
	c_pid = fork();
	int status;
	if(c_pid < 0){
		perror("*** error *** : fork ()");
		exit(-1);
	} 
	if(c_pid != 0){
		//i am parent
		//signal(SIGTSTP,SIG_IGN);
		
		add_job(c_pid,argument[0],RUNNIG);
		//printf("background ->>>>>>%d\n", background);
		usleep(25000);  
		if(background == 0){
				//printf(" parent   - %d\n", getpid());
				do 
		                {
		                    int wpid = waitpid(c_pid, &status, WUNTRACED | WCONTINUED);
		                    if (wpid == -1) 
		                    {
				                    perror("** error ** waitpid : ");
				                   //exit(EXIT_FAILURE);
		                        }

		                   if (WIFEXITED(status)) 
		                   {
		                    //printf("exited, status=%d\n", WEXITSTATUS(status));
		                    remove_pid(c_pid);
		                   }     
		                   else if (WIFSIGNALED(status)) 
		                   {
		                   // printf("killed by signal %d\n", WTERMSIG(status));
		                    remove_pid(c_pid);
		                   } 
		                   else if (WIFSTOPPED(status)) 
		                   {
		                  //  printf("process %d stopped by signal %d\n",(int)c_pid, WSTOPSIG(status));
		                   } 
		                   else if (WIFCONTINUED(status)) 
		                   {
		                       //printf("continued\n");
		                   }
		                } while (!WIFEXITED(status) && !WIFSIGNALED(status) && !WIFSTOPPED(status));
		}
		
	}
	else if(c_pid == 0){
		//i am child		
		//printf(" child   - %d\n", getpid());
		//sleep(10);
		
		char *outfilename;
		int fd; 
		int status;
		if(output_redirect == 1){
			int i=0;
			while(argument[i] != NULL){
				i++;
			}
			i--;
			outfilename = argument[i];
			argument[i]= NULL;
			fd = open(outfilename,O_APPEND|O_CREAT|O_WRONLY|O_TRUNC,0700);
			if(fd<0){
				perror("*** error *** : open() ");
				exit(1);
			}
			dup2(fd,1);
			close(fd);
		}
		else if(input_redirect == 1)
		{
			int i=0;
			while(argument[i] != NULL){
				i++;
			}
			i--;
			outfilename = argument[i];			
			argument[i]= NULL;
			fd = open(outfilename,O_RDONLY,0700);
			if(fd<0){
				perror("*** error *** : open() ");
				exit(1);
			}
				dup2(fd,0);
				close(fd);
		}
		int rr = execvp(argument[0],argument);
		if(rr == -1){
			printf("%s\n", "*** error *** : execvp");
		}
		exit(1);
	}
}

void execute_pipedCommand(char **argument){
		char *command2d[30][50];
		int q,r;
		for(q = 0; q < 30 ;q++){
			for(r = 0; r < 50 ;r++){
					command2d[q][r] = NULL;
			}
		}

		int z=0;
		int ptrRow = 0;
		int ptrCol = 0;
        while(argument[z] != NULL){
        	//printf("execute_pipedCommand - %s\n", argument[z]);
			if(strcmp(argument[z],"|") == 0){
				ptrRow++;
				ptrCol = 0;
				z++;
			}
			else{
				//printf("%s\n", "---------------------------------------------------------------");
				//printf("execute_pipedCommand - %s\n", argument[z]);
				command2d[ptrRow][ptrCol] = argument[z];
				ptrCol++;
				z++;
			}
		}
		//printf("%s\n", "array created");
		int i,j;
    	int pipe_count = 0;
    	int pipefdArray[30][2],newChild,status;
    	int fdforOutput;
        i= 0;
        while(i<count_piped){
            pipe(pipefdArray[i]);
            i++;
        }
    
    if(count_piped)
    {

         i = 0;
        while(i<=count_piped){
            
             newChild = fork();


            if(!newChild)
            {
                if(i!= 0)
                {
                    dup2(pipefdArray[i-1][0],0);
                }
                if(i!=count_piped)
                {
                    dup2(pipefdArray[i][1],1);
                }
                if(i == count_piped){
                    if(output_redirect == 1){
                    	//printf("*************************************************************");
                        int e=0;
                        while(command2d[i][e] != NULL){
                            e++;
                        }
                        e--;
                        char *outfilename = command2d[i][e];
                       // printf("output_File name %s\n", outfilename);
                        command2d[i][e] = NULL;
                        fdforOutput = open(outfilename,O_APPEND|O_CREAT|O_WRONLY|O_TRUNC,0700);
                        if( fdforOutput <0){
                            perror("*** error *** : open()");
                            exit(1);
                        }
                            dup2(fdforOutput,1);
                            close(fdforOutput);
                    }
                 }
                     j= 0;
                    while(j<count_piped){
                        close(pipefdArray[j][0]);
                        close(pipefdArray[j][1]);
                        j++;
                    }
                execvp(command2d[i][0], command2d[i]);
                exit(0);
            }

            i++;
        }



        i= 0;
        while(i<count_piped){
            close(pipefdArray[i][0]);
            close(pipefdArray[i][1]);
            i++;
        }

        if(background == 0 )
        		waitpid(newChild,&status,0);
    }
    else
    {
      //  execlp(string[0], string[0], NULL);
    }

}


int main(){

			master_process = getpid();
			init_jobs();
			initisBcg();
			install_signal_handler(SIGINT, int_handler);
			install_signal_handler(SIGTSTP, stp_handler);
			while(1){
				char *clear[] ={"clear",NULL};
				myshellpromt();
				char _command[500],c = '\0';
				c = getchar();

				if(c == '\n')
				{
					continue;
				}
				else {
								*(_command + 0) = c;
								readInput(_command);
								if(strcmp(_command,"exit") == 0){
									killpg(master_process,SIGKILL);
									exit(0);
								}
								else if(strcmp(_command,"cd") == 0){
									chdir(getenv(HOMEPATH));
								}
								else
								{
									char *argument[100];
									if(strlen(_command) > 0){
										generate_Path_arguments(argument,_command);
									}
									if(background == 1)
										{
											Add_to_isBcg();
											install_signal_handler(SIGCHLD, chld_handler);
										}
									if(strcmp(argument[0] , "kill") == 0){
											killProcess(atoi(argument[1]));
										}
									else if(strcmp(argument[0],"cd")==0){
												chdir(argument[1]);
										}
									else if(strcmp(argument[0] , "fg") == 0){
										char *c = argument[1];
										c++;
										int jobid = atoi(c);
										sendsigcont(jobid);
										}
									else if(strcmp(argument[0] , "jobs") == 0){
											print_jobs();
										}
									else if( piped_command == 1){
											execute_pipedCommand(argument);
										}
									else 
										execute(argument);	

								}
				}
		
				output_redirect = 0;
				input_redirect = 0;
				background = 0;
				piped_command = 0; 
				count_piped = 0;
			}

return 0;
}
