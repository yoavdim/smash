//		commands.c
//********************************************
#include "commands.h"
#include "jobs.h"



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
	int pid, status;
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

	}

	/*************************************************/
	else if (!strcmp(cmd, "pwd"))
	{

	}

	/*************************************************/
	else if (!strcmp(cmd, "mkdir"))
	{

	}
	/*************************************************/

	else if (!strcmp(cmd, "jobs"))
	{
			jobs_print_all(jobs);
	}
	/*************************************************/
	else if (!strcmp(cmd, "showpid"))
	{

	}
	/*************************************************/
	else if (!strcmp(cmd, "fg"))
	{

	}
	/*************************************************/
	else if (!strcmp(cmd, "bg"))
	{

	}
	/*************************************************/
	else if (!strcmp(cmd, "quit"))
	{
		if(args[1]) {
			if(!strcmp(args[1], "kill")) {
				// do the killing & then exit
			} else {
				// error?
			}
		} else {
			exit(0);
		}
	}
	/*************************************************/
	else // external command
	{
 		pid = ExeExternal(args, cmdString);
		if(pid <= 0) return -1;
		if(waitpid(pid, &status, WUNTRACED) != pid){
			perror("Failed waiting for the command");
			return -1;
		} else {
			if(WIFSTOPPED(status))
				if(jobs_add(jobs, pid, lineSize) < 0) {
					perror("Failed to add job to list");
					return -1;
				}
		}
	 	return 0;
	}
	if (illegal_cmd == TRUE)
	{
		printf("smash error: > \"%s\"\n", cmdString);
		return 1;
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
		pid = ExeExternal(args,Command);
		if(pid > 0){
			if(jobs_add(jobs, pid, lineSize))
				return 0;
		}

	}
	return -1;
}
