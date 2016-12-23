**************************************************************************************************************
								Assignment 1 : CS-550 / Operating System 
**************************************************************************************************************
Group Members:-
	1) Name  : Prashant Kadam
	   Email : pkadam1@binghamton.edu

	2) Name  : Pranav Pitale
	   Email : ppitale1@binghamton.edu
**************************************************************************************************************


1)Compile the myshell.c by running make command in the specificed directory OS_Assignment1
2)Run the executable by entering command myshell
	will get following promt -> 
	minishell :~> 
3)Minishell accepts command in following format (space seperated)
	>ls -l
	>pwd
	>sort < input | wc > out.txt
	>ls &
	There must be space between command and the argument.Space must be between > ,& and | 
4)Minishell can be terminated using exit command.

Mostly Used System calls - 
	fork()
	execvp()
	waitpid()
	dup2() (for stdin/stdout redirection)
	pipe()
	open()
	close()
	kill()
	chdir()
	Signal() - 
	


