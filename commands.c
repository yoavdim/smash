//		commands.c
//********************************************
#include "commands.h"
#include "jobs.h"


int wait_job(void* jobs, int pid, char const* line);
int fg_pid = 0;

//my add
char last_cd1[MAX_LINE_SIZE];
char last_cd2[MAX_LINE_SIZE];
int flag = 0 ;

int ih =0 ;
char history[50][MAX_LINE_SIZE]; //

//********************************************
// function name: ExeCmd
// Description: interperts and executes built-in commands
// Parameters: pointer to jobs, command string
// Returns: 0 - success,1 - failure
//**************************************************************************************
int ExeCmd(void* jobs, char* lineSize, char* cmdString)
{
	char* cmd;
	char* args[MAX_ARG];
	char pwd[MAX_LINE_SIZE];
	char* delimiters = " \t\n";
	int i = 0, num_arg = 0;
	bool illegal_cmd = FALSE; // illegal command
	int pid, id, signum;
	job_t job_result;

	// for history command
	if(ih<50){
		strcpy(history[ih], cmdString);
		ih = ih +1 ;
	}else{
		for(int i=0 ;i<49;i++){
			strcpy(history[i], history[i+1]);
		}
		strcpy(history[49], cmdString);
	}

	cmd = strtok(lineSize, delimiters);
	if (cmd == NULL)
		return 0;
	args[0] = cmd;
	for (i=1; i<MAX_ARG; i++)
	{
		args[i] = strtok(NULL, delimiters);
		if (args[i] != NULL)
			num_arg++;

	}
	/*************************************************/
	// Built in Commands PLEASE NOTE NOT ALL REQUIRED
	// ARE IN THIS CHAIN OF IF COMMANDS. PLEASE ADD
	// MORE IF STATEMENTS AS REQUIRED
	/*************************************************/
	if (!strcmp(cmd, "cd") )
	{

		if(*args[1]!='-'){
			getcwd(last_cd1, MAX_LINE_SIZE);
			flag=1;

			int ret = chdir(args[1]);
			if(ret==0){
				//printf(" last_cd not ----\n");

			}else { // ret!=0 ... error
				printf("smash error: > “%s” – No such file or directory",cmdString);
				return 1;
			}
		}
		if(*args[1]=='-'){

			if(flag){
				getcwd(last_cd2, MAX_LINE_SIZE);
				int ret =chdir(last_cd1);
				strcpy(last_cd1, last_cd2);

				if(ret==0){
					//printf(" chdir okayy");
				}

			}else{ // last_cd == NULL
				printf("smash error: > “%s” – No such file or directory",cmdString);
				return 1;
			}

		}
	}

	/*************************************************/
	else if (!strcmp(cmd, "pwd"))
	{
		char *token;
		char *last_token;
		const char s[2] = "/";
		getcwd(pwd, MAX_LINE_SIZE);

		/* get the first token */
		token = strtok(pwd, s);

		/* walk through other tokens */
		while( token != NULL ) {

			last_token=token;
			token = strtok(NULL, s);
		}

		printf("%s%s\n",s, last_token);
	}
	/*************************************************/
	else if (!strcmp(cmd, "history"))
	{

		//printf("ih ==%d \n",ih);
		int n_ih = 0;
		while(n_ih!=ih+1){
			printf("%s\n",history[n_ih]);
			n_ih=n_ih+1;
		}
	}
	/*************************************************/
	else if (!strcmp(cmd, "kill"))
	{
		if(num_arg == 2 && args[1][0] == '-' && args[1][1] != '\0') {
			id = atoi(args[2]);
			job_result = jobs_get_id(jobs, id);
			pid = job_result.pid;
			if(pid > 0){
				signum = atoi(args[1] + 1);
				if(signum>0) {
					if(!kill(pid, signum)) {
						return 0;
					}
				}
				printf("smash error: > kill %d - cannot send signal\n", id);
				return 1;
			} else {
				printf("smash error: > kill %d - job does not exist\n", id);
				return 1;
			}
		} else {
			illegal_cmd = TRUE;
		}
	}
	/*************************************************/

	else if (!strcmp(cmd, "jobs"))
	{
		jobs_print_all(jobs);
	}
	/*************************************************/
	else if (!strcmp(cmd, "diff"))
	{
		int dif = 0 ;
		char str1[MAX_LINE_SIZE] ;
		char str2[MAX_LINE_SIZE] ;
		FILE * fd1 = fopen(args[1],"r");
		if(fd1==NULL){
			perror("failed in open file");
			return 1;
		}
		FILE * fd2 = fopen(args[2],"r");
		if(fd2==NULL){
			perror("failed in open file");
			fclose(fd1);
			return 1;
		}
		while(fgets(str1, MAX_LINE_SIZE, fd1)){
			fgets(str2, MAX_LINE_SIZE, fd2);
			if (strcmp(str1, str2)){
				dif =1;
			}
		}
		if(fgets(str2, MAX_LINE_SIZE, fd2)){
			dif =1;
		}
		printf("%d\n", dif);
		fclose(fd1);
		fclose(fd2);
	}
	/*************************************************/
	else if (!strcmp(cmd, "showpid"))
	{
		int pid = getpid() ;
		printf("smash pid is %d\n",pid);
	}
	/*************************************************/
	else if (!strcmp(cmd, "fg"))
	{
		switch(num_arg){
			case 0:
				job_result = jobs_get_last(jobs, 0);
				break;
			case 1:
				id = atoi(args[1]);
				job_result = jobs_get_id(jobs, id);
				break;
			default:
				illegal_cmd = TRUE;
				break;
		}
		if(illegal_cmd != TRUE && job_result.pid > 0) {
			if(! job_result.running) {
				if(kill(job_result.pid, SIGCONT)) {
					perror("failed kill in bg");
				}
			}
			strcpy(pwd, job_result.name);
			printf("%s",job_result.name);
			jobs_remove(jobs, job_result.id);
			wait_job(jobs, job_result.pid, pwd);
		} else {
			illegal_cmd = TRUE;
		}
	}
	/*************************************************/
	else if (!strcmp(cmd, "bg"))
	{

		switch(num_arg){
			case 0:
				job_result = jobs_get_last(jobs, 1);
				break;
			case 1:
				id = atoi(args[1]);
				job_result = jobs_get_id(jobs, id);
				break;
			default:
				illegal_cmd = TRUE;
				break;
		}
		if(illegal_cmd != TRUE && job_result.pid > 0) {
			if(! job_result.running) {
				if(kill(job_result.pid, SIGCONT)) {
					perror("failed kill in bg");
				} else {
					printf("%s",job_result.name);
				}
			}
		} else {
			illegal_cmd = TRUE;
		}
	}
	/*************************************************/
	else if (!strcmp(cmd, "quit"))
	{
		if(args[1]) {
			if(!strcmp(args[1], "kill")) {
				jobs_kill_all(jobs);
				exit(0);
			} else {
				illegal_cmd = TRUE;
			}
		} else {
			exit(0);
		}
	}
	/*************************************************/
	else // external command
	{
		pid = ExeExternal(args, cmdString);
		if(pid <= 0)
			return 1;

		if(wait_job(jobs, pid, lineSize) < 0)
			return 1;

		return 0;
	}

	if (illegal_cmd == TRUE)
	{
		printf("smash error: > \"%s\"\n", cmdString);
		return 1;
	}
	return 0;
}

int wait_job(void* jobs, int pid, char const* line) {
	int id;
	siginfo_t infop = {};

	if(pid <= 0)
		return -1;

	while(1) {
		fg_pid = pid;
		if (waitid(P_PID, pid, &infop, WUNTRACED|WNOWAIT|WEXITED)) {
			if(errno == EINTR)
				continue;
			fg_pid = 0;
			perror("Failed waiting for the command");
			return -1;
		} else {
			fg_pid = 0;
			if(infop.si_code == CLD_STOPPED) {
				id = jobs_add(jobs, pid, line);
				if(id <= 0) {
					perror("Failed to add job to list");
					return -1;
				} else {
					return id;
				}
			} else {
				// pid was ended and not just stopped, clean it
				if(waitpid(pid, NULL, WNOHANG) != pid) {
					printf("%s: error: expected to be zommbie\n", __func__);
				}
			}
		}
	}
	return 0;
}
//**************************************************************************************
// function name: ExeExternal
// Description: executes external command,
// Parameters: external command arguments, external command string
// Returns: pid
//**************************************************************************************
int ExeExternal(char *args[MAX_ARG], char* cmdString)
{
	int pID;
	switch(pID = fork())
	{
		case -1:
			// Add your code here (error)
			perror("Failed spawning a new process");
			return -1;
		case 0 :
			// Child Process
			setpgrp();

			// Add your code here (execute an external command)
			execv(cmdString, args);
			perror("Failed running an external command");
			exit(1);

		default:
			// Add your code here
			return pID;
	}
}

//**************************************************************************************
// function name: BgCmd
// Description: if command is in background, insert the command to jobs
// Parameters: command string, pointer to jobs
// Returns: 0- BG command -1- if not
//**************************************************************************************
int BgCmd(char* lineSize, void* jobs)
{

	char* Command;
	char* delimiters = " \t\n";
	char *args[MAX_ARG];
	int i = 0, num_arg = 0;
	int pid;
	if (lineSize[strlen(lineSize)-2] == '&')
	{
		lineSize[strlen(lineSize)-2] = '\0';
		// Add your code here (execute a in the background)
		Command = strtok(lineSize, delimiters);
		if (Command == NULL)
			return 0;

		args[0] = Command;
		for (i=1; i<MAX_ARG; i++)
		{
			args[i] = strtok(NULL, delimiters);
			if (args[i] != NULL)
				num_arg++;
		}

		// my code:
		pid = ExeExternal(args, Command);
		if(pid > 0){
			if(jobs_add(jobs, pid, lineSize))
				return 0;
			else
				printf("Failed to add job to queue\n");
		} else {
			printf("%s: Failed to run command\n", __func__);
		}
	}
	return -1;
}
