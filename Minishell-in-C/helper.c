#define JOBLIMIT 100
#define BCGPTRACK 30

struct job_list{
	 pid_t pid;
    int jobid;
    char cmd[15];
    char state;
};



int isBcg[BCGPTRACK];

static int job_id_master = 1;
static struct job_list jobs[JOBLIMIT];

void init_jobs(){
	int i =0;
	for(i=0;i<JOBLIMIT;i++){
		jobs[i].pid = 0;
		jobs[i].jobid = 0;
		jobs[i].state = '\0';
		memset(jobs[i].cmd,'\0',15);
	}
}

void add_job(pid_t pid,char* cmd,char state){
	//printf("----%s\n", cmd);

		int i =0;
		for(i=0;i<JOBLIMIT;i++){
			if(jobs[i].jobid == 0){
				jobs[i].jobid = job_id_master;
				jobs[i].pid = pid;
				jobs[i].state = state;
				strcpy(jobs[i].cmd,cmd);
				job_id_master++;
				break;
			}
		}
}
void updateJobStatus(pid_t	processId, char state){
int i =0;
		for(i=0;i<JOBLIMIT;i++){
			if(jobs[i].pid == processId){
				jobs[i].state = state;
				break;
			}
		}
}
void remove_job(int jobid){
		int i =0;
		for(i=0;i<JOBLIMIT;i++){
			if(jobs[i].jobid == jobid){
				jobs[i].jobid = 0;
				jobs[i].pid = 0;
				jobs[i].state = '\0';
				memset(jobs[i].cmd,'\0',15);
				job_id_master--;
			}
		}
}

pid_t getProcessId(int jobid){
	int i;
	for(i=0;i<JOBLIMIT;i++){
			if(jobs[i].jobid == jobid){
				return jobs[i].pid;
			}
		}	

}
void remove_pid(pid_t pid){
	//printf("%s\n", "In remove_pid");
		int i =0;
		for(i=0;i<JOBLIMIT;i++){
			if(jobs[i].pid == pid){
				jobs[i].jobid = 0;
				jobs[i].pid = 0;
				jobs[i].state = '\0';
				job_id_master--;
			}
		}

		/*for(i=0;i<JOBLIMIT;i++){
			if(jobs[i].state == 'R'){
				is
			}
		}*/
}

void print_jobs(){
	int i =0;
	//printf("%s\n", "--------------------------------------Jobs-------------------------------------");
	//printf("%s\n", jobs[0].cmd);
	printf("%s\n", "-----------------------------------------");
	printf("%s    %s    %s    %s\n","job id","process id","command","state");
	printf("%s\n", "-----------------------------------------");
		for(i=0;i<JOBLIMIT;i++){
			if(jobs[i].jobid != 0){
				printf("%d\t  %d\t\t%s     %c\n",jobs[i].jobid,jobs[i].pid,jobs[i].cmd,jobs[i].state);
			}
			
		}
	printf("%s\n", "-----------------------------------------");
	///printf("%s\n", "--------------------------------Jobs-------------------------------");
}


void initisBcg(){
	//printf("%s\n", "initisBcg");
	int i;
	for(i = 0;i < BCGPTRACK;i++){
		isBcg[i] = -1;
	}
}
void Add_to_isBcg(){
//	printf("%s\n", "Add_to_isBcg");
	int i;
	for(i = 0;i < BCGPTRACK;i++){
		if(isBcg[i] == -1)
			{
				isBcg[i] = 1;
				break;
			}
	}
}
void remove_last_isBcg(){
	//printf("%s\n", "remove_last_isBcg");
	int i;
	for(i = BCGPTRACK - 1;i >= 0;i--){
		if(isBcg[i] == 1)
			{
				isBcg[i] = 0;
				break;
			}
	}
}

int is_any_bcgSet(){
	//printf("%s\n", "is_any_bcgSet");
	int i;
	for(i = 0;i < BCGPTRACK;i++){
		if(isBcg[i] == 1)
			{
				return 1;
			}
	}
	return 0;
}