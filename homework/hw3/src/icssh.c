#include "icssh.h"
#include "helpers.h"
#include <readline/readline.h>
#include <linkedList.h>

volatile sig_atomic_t chld_term = 0;

void sigchld_handler(int sig) {
	chld_term = 1;
}

void sigusr2_handler(int sig) {
	fprintf(stdout, "Hi User! I am process %d\n", getpid());
}

int main(int argc, char* argv[]) {
	int exec_result;
	int exit_status;
	pid_t pid;
	pid_t pid2;
	pid_t pid3;
	pid_t wait_result;
	char* line;
	time_t seconds;
	List_t *bgjobs = malloc(sizeof(List_t));
	bgjobs->head = NULL;
	bgjobs->length = 0;
	bool bg_op;
	int piping = 0;
#ifdef GS
    rl_outstream = fopen("/dev/null", "w");
#endif

	// Setup segmentation fault handler
	if (signal(SIGSEGV, sigsegv_handler) == SIG_ERR) {
		perror("Failed to set signal handler");
		exit(EXIT_FAILURE);
	}
	
	// Setup SIGCHLD handler
	if (signal(SIGCHLD, sigchld_handler) == SIG_ERR) {
		perror("Failed to set signal handler");
		exit(EXIT_FAILURE);
	}

	// Setup SIGUSR2 handler
	if (signal(SIGUSR2, sigusr2_handler) == SIG_ERR) {
		perror("Failed to set signal handler");
		exit(EXIT_FAILURE);
	}

    	// print the prompt & wait for the user to enter commands string
	while ((line = readline(SHELL_PROMPT)) != NULL) {
        	// MAGIC HAPPENS! Command string is parsed into a job struct
        	// Will print out error message if command string is invalid
		job_info* job = validate_input(line);
        	if (job == NULL) { // Command was empty string or invalid
			free(line);
			continue;
		}
		
		seconds = time(NULL);			// time of command

        	//Prints out the job linked list struture for debugging
        	#ifdef DEBUG   // If DEBUG flag removed in makefile, this will not longer print
            		debug_print_job(job);
        	#endif


		// error handling
		if (job->in_file != NULL && job->out_file != NULL) {
			if (strcmp(job->in_file, job->out_file) == 0) {
				fprintf(stderr, "%s", RD_ERR);
				free(line);
				free_job(job);
				deleteList(bgjobs);
				validate_input(NULL);
				exit(EXIT_FAILURE);
			}
		}


// BUILT-INs
		// example built-in: exit
		if (strcmp(job->procs->cmd, "exit") == 0) {
			// Terminating the shell
			node_t *head = bgjobs->head;
			if (head == NULL) {
				free(line);
				free_job(job);
				deleteList(bgjobs);
				free(bgjobs);
				validate_input(NULL);   // calling validate_input with NULL will free the memory it has allocated
				return 0;
			}
			bgentry_t *current = head->value;
			while(1) {
				kill(current->pid, SIGKILL);
				fprintf(stdout, BG_TERM, current->pid, current->job->line);
				head = head->next;
				if (head == NULL) {
					break;
				}
				current = head->value;
			}
								
			free(line);
			free_job(job);
			deleteList(bgjobs);
			free(bgjobs);
            		validate_input(NULL);   // calling validate_input with NULL will free the memory it has allocated
			return 0;
		}

		// built-in: cd
		if (strcmp(job->procs->cmd, "cd") == 0) {
			//printf("%s\n", job->procs->argv[1]);
	
			const char *dir;
			if (job->procs->argc == 1) {			// NO dir given
				dir = getenv("HOME");
			} else {
				dir = job->procs->argv[1];
			}
			int r = cd(dir);

			if (r == 0) {		//success
				char s[100];
				fprintf(stdout, "%s\n", getcwd(s,100));
			} else {
				fprintf(stderr, "%s", DIR_ERR);
			}
			free(line);
			free_job(job);
			continue;
		}

		//built-in: estatus
		if (strcmp(job->procs->cmd, "estatus") == 0) {
			printf("%d\n", WEXITSTATUS(exit_status));
			free(line);
			free_job(job);
			continue;
		}

		//built-in: bglist
		if (strcmp(job->procs->cmd, "bglist") == 0) {
			node_t *head = bgjobs->head;
			if (head == NULL) {
				free(line);
				free_job(job);
				continue;
			}
			bgentry_t *current = head->value;
			while(1) {
				print_bgentry(current);
				head = head->next;
				if (head == NULL) {
					break;
				}
				current = head->value;
			}
			free(line);
			free_job(job);
			continue;
		}

// Not BUILT-INs

		bg_op = job->bg;		// set flag for bg jobs
		if (job->nproc > 1)
			piping = 1;
		
		int pipefd1[2];
		int pipefd2[2];

		// Pipes
		if (piping == 1) {
			if (job->nproc == 2){		// Create 1 pipe
			//	int pipefd1[2];
				if (pipe(pipefd1) < 0) {
					fprintf(stderr, "%s", PIPE_ERR);
					exit(EXIT_FAILURE);
				}
			}
			/**/
			else if(job->nproc == 3) {		// Create 2 pipes
			//	int pipefd1[2];
			//	int pipefd2[2];
				if (pipe(pipefd1) < 0) {
					fprintf(stderr, "%s", PIPE_ERR);
					exit(EXIT_FAILURE);
				}
				if (pipe(pipefd2) < 0) {
					fprintf(stderr, "%s", PIPE_ERR);
					exit(EXIT_FAILURE);
				}	
			}
			/**/
		}

		// example of good error handling!
		if ((pid = fork()) < 0) {
			perror("fork error");
			exit(EXIT_FAILURE);
		}
		if (pid == 0) {  //If zero, then it's the child process 1
			if(piping == 1) {		// set pipe for pid
				if (job->nproc == 2) {		// one pipe
					close(pipefd1[0]);
					dup2(pipefd1[1], STDOUT_FILENO);
					//
					//close(pipefd1[1]);
				}
				 
				else if (job->nproc == 3) {
					close(pipefd1[0]);
					//close(pipefd2[0]);
					//close(pipefd2[1]);
					dup2(pipefd1[1], STDOUT_FILENO);
					//
					//close(pipefd1[1]);
				}
				
					
			} else {
				// set stdin/stdout/stderr if needed
				if (job->in_file != NULL) {				// sets stdin
					int fd0 = open(job->in_file, O_RDONLY);
					if (fd0 == -1) {
						fprintf(stderr, "%s", RD_ERR);
						free(line);
						free_job(job);
						deleteList(bgjobs);
						validate_input(NULL);
						exit(EXIT_FAILURE);
					}	
					dup2(fd0, STDIN_FILENO);
					close(fd0);
				}

				if (job->out_file != NULL) {				// sets stdout
					int fd1 = open(job->out_file, O_WRONLY | O_CREAT | O_EXCL);
					if (fd1 == -1) {
						fd1 = open(job->out_file, O_WRONLY);
					}
					dup2(fd1, STDOUT_FILENO);
					close(fd1);
				}

				if (job->procs->err_file != NULL) {			// sets stderr
					int fd2 = open(job->procs->err_file, O_WRONLY | O_CREAT | O_EXCL);
					if (fd2 == -1) {
						fd2 = open(job->procs->err_file, O_WRONLY);
					}
					dup2(fd2, STDERR_FILENO);
					close(fd2);
				}
			}

			//get the first command in the job list
		    proc_info* proc = job->procs;
			exec_result = execvp(proc->cmd, proc->argv);
			if (exec_result < 0) {  //Error checking
				printf(EXEC_ERR, proc->cmd);
				
				// Cleaning up to make Valgrind happy 
				// (not necessary because child will exit. Resources will be reaped by parent)
				free_job(job);  
				free(line);
				deleteList(bgjobs);
    				validate_input(NULL);  // calling validate_input with NULL will free the memory it has allocated

				exit(EXIT_FAILURE);
			}
			//close(pipefd1[1]);
		}


		// create more child proccesses 
		if (piping == 1) {
			if (job->nproc == 2) {		// create 1 more child
				if ((pid2 = fork()) < 0) {
					perror("fork error");
					exit(EXIT_FAILURE);
				}
				if (pid2 == 0) {  //If zero, then it's the child process 2
					close(pipefd1[1]);
					dup2(pipefd1[0], STDIN_FILENO);
					//
					//close(pipefd1[0]);	

					proc_info* proc = job->procs;
					proc = proc->next_proc;
					exec_result = execvp(proc->cmd, proc->argv);
					if (exec_result < 0) {	// Error checking
						printf(EXEC_ERR, proc->cmd);

						// Clean up
						free_job(job);
						free(line);
						deleteList(bgjobs);
						validate_input(NULL);
						exit(EXIT_FAILURE);
					}
					//close(pipefd1[0]);
				}
				close(pipefd1[0]);
				close(pipefd1[1]);
			}
			/**/
			else if (job->nproc == 3) {		// create 2 more children
				if ((pid2 = fork()) < 0) {
					perror("fork error");
					exit(EXIT_FAILURE);
				}
				if (pid2 == 0) { //child process 2
					close(pipefd1[1]);
					close(pipefd2[0]);
					dup2(pipefd1[0], STDIN_FILENO);
					dup2(pipefd2[1], STDOUT_FILENO);
					//
					//close(pipefd1[0]);
					//close(pipefd2[1]);

					proc_info* proc = job->procs;
					proc = proc->next_proc;
					exec_result = execvp(proc->cmd, proc->argv);
					if (exec_result < 0) {	// Error checking
						printf(EXEC_ERR, proc->cmd);

						// Clean up
						free_job(job);
						free(line);
						deleteList(bgjobs);
						validate_input(NULL);
						exit(EXIT_FAILURE);
					}
				}
				//close(pipefd1[0]);
				//close(pipefd2[1]);
				//
				close(pipefd1[0]);
				close(pipefd1[1]);

				if ((pid3 = fork()) < 0) {
					perror("fork error");
					exit(EXIT_FAILURE);
				}
				if (pid3 == 0) {	//child process 3
					//close(pipefd1[0]);
					//close(pipefd1[1]);
					close(pipefd2[1]);
					dup2(pipefd2[0], STDIN_FILENO);
					//
					//close(pipefd2[0]);

					proc_info* proc = job->procs;
					proc = proc->next_proc; //process 2
					proc = proc->next_proc; //process 3 
					exec_result = execvp(proc->cmd, proc->argv);
					if (exec_result < 0) {	// Error checking
						printf(EXEC_ERR, proc->cmd);

						// Clean up
						free_job(job);
						free(line);
						deleteList(bgjobs);
						validate_input(NULL);
						exit(EXIT_FAILURE);
					}
				}
				close(pipefd2[0]);
				close(pipefd2[1]);
			}
			/**/
		}		
	
		if (piping == 1) {
			if (job->nproc == 2) {
				wait_result = waitpid(pid2, &exit_status, 0);
				if (wait_result < 0) {
					printf(WAIT_ERR);
					exit(EXIT_FAILURE);
				}
			}
			else if (job->nproc == 3) {
				wait_result = waitpid(pid3, &exit_status, 0);
				if (wait_result < 0) {
					printf(WAIT_ERR);
					exit(EXIT_FAILURE);
				}
			}
		}			


		if (bg_op == 1) {			//background process	
			//printf("bg\n");
			bgentry_t *bgjob = malloc(sizeof(bgentry_t));
			bgjob->job = job;
			bgjob->pid = pid;
			bgjob->seconds = seconds;
			
			insertRear(bgjobs, bgjob);		// add bg job to linked list
			
			/*
			wait_result = waitpid(pid, &exit_status, WNOHANG); 
			if (wait_result < 0) {
				printf(WAIT_ERR);
				exit(EXIT_FAILURE);
			}
			*/
			free(line);
			continue;
		} else {
			// As the parent, wait for the foreground job to finish
			wait_result = waitpid(pid, &exit_status, 0);
			if (wait_result < 0) {
				printf(WAIT_ERR);
				exit(EXIT_FAILURE);
			}

			// check/reap child processes
			if (chld_term == 1) {
				int corpse;
				while ((corpse = waitpid(-1, &exit_status, WNOHANG)) > 0) {
					// remove from list
					int counter = 0;
					node_t *head = bgjobs->head;
					if (head == NULL) {
						break;
					}
					bgentry_t *current = head->value;
					while(1) {
						if (current->pid == corpse) {
							//print
							job_info *j = current->job;
							if (j->bg == 1) {
								fprintf(stdout, BG_TERM, corpse, j->line);
								free_job(current->job);
								removeByIndex(bgjobs, counter);
							}	
							break;
						} else {
							counter++;
							head = head->next;
							if (head == NULL) {
								break;
							}
							current = head->value;
						}
					}
					free(current);
				}
				chld_term = 0;
			}
		}

		free_job(job);  // if a foreground job, we no longer need the data
		free(line);
	}

    	// calling validate_input with NULL will free the memory it has allocated
    	validate_input(NULL);
	deleteList(bgjobs);
	free(bgjobs);

#ifndef GS
	fclose(rl_outstream);
#endif
	return 0;
}
