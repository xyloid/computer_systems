#include <ctype.h>
#include <errno.h>
#include <signal.h> // This is new!
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define BUFFER_SIZE 80
// Because of space delimiters, the max number of arguments can be estimated.
#define ARG_NUM (BUFFER_SIZE + 1) / 2 + 1

// Size of the history ring buffer.
// I set a small number here so I can easily test it.
#define HIST_MAX 11

// Data struct of the history ring buffer.
typedef struct record {
	char *log[HIST_MAX]; // Pointers to the buf. We put the buffer on the stack,
	                     // so it's malloc free.
	int index;           // Current index in the array.
	int total_count;     // Total number of the commands typed.
} record_t;

// In fact, history will only add (or overwrite) items. no remove, no deletion.
int add_log(record_t *r, char *cmd) {
	if (r) {
		int i = r->index % HIST_MAX;
		memset(r->log[i], '\0', sizeof(char) * BUFFER_SIZE);
		strcpy(r->log[i], cmd);
		r->index++;
		r->total_count++;
		return 0;
	} else {
		// Return on null pointer.
		return -1;
	}
}

// Built-in function struct. This struct is used to represent a built-in
// command.
typedef struct function {
	char *name;             // name of the built-in command.
	int (*p_func)(char **); // function pointer of the command.
} cmd_t;

// Create a signal handler
void sigint_handler(int sig) {
	// Kill all the child processes.
	kill(0, SIGINT);

	// Ask yourself why 35? The length of the string.
	// 1 represents stdout.
	write(1, "Terminating through signal handler\n", 35);
	exit(0);
}

// Parse the input string so execvp can understand it.
// input is the string to be parsed.
// delimiter is used to specify delimiter for strtok.
// result is the pointers for the result.
// buf is used to provide memory space for the result.
void parse(char *input, char *delimiter, char **result,
           char buf[][BUFFER_SIZE]) {
	char *token = strtok(input, delimiter);
	int arg_count = 0;
	// Each argument is a char array.
	while (token != NULL) {
		result[arg_count] = buf[arg_count];
		strcpy(result[arg_count++], token);
		token = strtok(NULL, delimiter);
	}
	// Mark the end of the arguments.
	result[arg_count] = NULL;
}

// argv ends with NULL.
int cmd_help(char **argv) {
	printf(
	    "Help of mini-shell bulitin commands:\n"
	    "0) help:\t print the help of the builtin commands.\n"
	    "1) exit:\t exit mini-shell by this command. You can also exit "
	    "mini-shell at anytime by Ctrl+c.\n"
	    "2) cd: \t change the current directory to the specified directory by "
	    "'cd dir'.\n"
	    "3) hist:\t hist [num] print the history of the commands the user "
	    "typed. It will print at most %d commands with a number denoting "
	    "the order it was typed. You can also specify the number of "
	    "commands to be printed by an optional argument [num].\n",
	    HIST_MAX - 1);
	return 0;
}

// Used to store history commands.
static record_t history;

int cmd_history(char **argv) {
	// Check invalid argument number.
	if (argv[2] != NULL) {
		printf("Error: hist takes at most 1 arguments.\n");
		return -1;
	}
	// Record HIST_MAX history in ring buffer.
	int size = HIST_MAX < history.total_count ? HIST_MAX : history.total_count;

	// After hist cmd, the record size is at least 1.
	if (size == 1) {
		printf("No previous command.\n");
		return 0;
	}

	// Check if argument 1 is an int within the correct range.
	if (argv[1] != NULL) {
		// Parse the number.
		int num = atoi(argv[1]);
		if (num <= 0 || num >= HIST_MAX) {
			printf("Error: Invalid number of history commands. It should be in "
			       "[%d, %d]\n",
			       0, HIST_MAX);
			return -1;
		}
		num++;
		size = num < size ? num : size;
	}
	int i;
	for (i = size; i > 1; i--) {
		printf("No.%4d ", history.total_count - i);
		printf("%s", history.log[(history.index - i + HIST_MAX) % HIST_MAX]);
	}
	return 0;
}

// Used to nullify an array of pointers by setting each pointer to NULL.
void nullify_pointer_array(char **pointers, int size) {
	while (size-- > 0) {
		*(pointers + size) = NULL;
	}
}

// This function is used to call general builtin functions. A general builtin
// function has io and will be called in a child process like an external
// command. On the other hand, a special builtin command (cd, exit) will be
// "executed" in the parent process.
// This approach makes it possible to use a pipe to redirect help or hist
// command's output to the input of grep.
int builtin(char **cmd, cmd_t **cmdset) {
	if (cmd && cmdset) {
		int i = -1;
		while (cmdset[++i] != NULL) {
			if (strcmp(cmd[0], cmdset[i]->name) == 0) {
				// Call function and return its return value.
				return cmdset[i]->p_func(cmd);
			}
		}
	}
	return -1;
}

// Check for empty string. It used to prevent empty string in history.
int is_not_empty(char *s) {
	while (*s) {
		if (!isspace(*s)) {
			return 1;
		}
		s++;
	}
	return 0;
}

int main() {
	// Generate a process group. All the child processes belongs to this process
	// group. They will be killed by kill(0, SIGINT).
	int pgid = setpgid(0, 0);
	if (pgid == -1) {
		perror("Failed tp setpgid.");
		exit(1);
	}
	// Initialize history record.
	// Use a buffer to avoid malloc.
	char hist_record_buffer[HIST_MAX][BUFFER_SIZE];
	history.total_count = 0;
	history.index = 0;
	int i = 0;
	for (i = 0; i < HIST_MAX; i++) {
		history.log[i] = hist_record_buffer[i];
	}

	// Initial built in command.
	cmd_t *cmd[3];
	cmd_t builtin_help = {"help", cmd_help};
	cmd_t builtin_hist = {"hist", cmd_history};
	cmd[0] = &builtin_help;
	cmd[1] = &builtin_hist;
	cmd[2] = NULL;

	// Buffer initialization.
	char buffer[BUFFER_SIZE];
	// Max length of a single command name is BUFFER_SIZE
	// Max number of arguments must be smaller than BUFFER_SIZE, as it needs a
	// space between command name and arguments. In fact, if each argument is
	// only one character length, then it could have at most: 39 arguments + 39
	// spaces + 1 command = 79 characters. So, it could have at most 39
	// arguments + 1 command + 1 NULL = 41 char array in parse_result.
	char parse_result_buffer[ARG_NUM][BUFFER_SIZE];
	char *parse_result[ARG_NUM];
	nullify_pointer_array(parse_result, ARG_NUM);

	// Split one command into several commands if there is any.
	char split_result_buffer[ARG_NUM][BUFFER_SIZE];
	char *split_result[ARG_NUM];
	nullify_pointer_array(split_result, ARG_NUM);

	// Two pipes will be used alternatively if there are more than one pipes in
	// one command. fd_even_to_odd: pass STDOUT of cmd 2n to STDIN of cmd 2n+1.
	// fd_odd_to_even: pass STDOUT of cmd 2n+1 to STDIN of cmd 2n+2.
	int fd_even_to_odd[2];
	int fd_odd_to_even[2];
	int cmd_index;
	// Install our signal handler.
	signal(SIGINT, sigint_handler);

	printf("You can only terminate by pressing Ctrl+C\n");

	while (1) {
		printf("mini-shell>");
		// Clean buffer.
		memset(buffer, '\0', sizeof(char) * BUFFER_SIZE);
		fgets(buffer, BUFFER_SIZE, stdin);
		if (ferror(stdin)) {
			perror("Input Error: ");
		}
		// Check if command is empty.
		if (is_not_empty(buffer)) {
			// Record non empty command string.
			add_log(&history, buffer);
		} else {
			// Empty command, continue to next command.
			continue;
		}

		// Write to history.
		// add_log(&history, buffer_bk);

		// Remove tail '\n'.
		buffer[strcspn(buffer, "\n")] = 0;
		// Split by "|", and parse each command one by one.
		parse(buffer, "|", split_result, split_result_buffer);

		// If there is more than one single command, we need a pipe.
		if (split_result[1] != NULL) {
			pipe(fd_even_to_odd);
		}
		cmd_index = 0;
		// I have implemented a strategy that uses 2 pipes to execute command
		// with multiple '|'s. So we can use the following command:
		// mini-shell>ls | grep c | grep s | grep h | grep e | grep l
		// mini-shell>help | grep a | grep num
		while (split_result[cmd_index] != NULL) {

			parse(split_result[cmd_index], " ", parse_result,
			      parse_result_buffer);
			// In case empty input.
			if (parse_result[0] == NULL) {
				printf("Error: Empty command found!\n");
				break;
			}

			// Some special builtin commands.

			if (strcmp(parse_result[0], "exit") == 0) {

				// CMD exit

				// Check if there is any arguments.
				if (parse_result[1] != NULL) {
					printf("Error: exit doesn't take any argument.\n");
					break;
				} else {
					// clean and exit;
					printf("Leaving mini shell.\n");
					return 0;
				}
			} else if (strcmp(parse_result[0], "cd") == 0) {

				// CMD cd

				if (parse_result[2] != NULL || parse_result[1] == NULL) {
					printf("Error: cd must have exactly one argument.\n");
					break;
				} else {
					// Call chdir in the parent process.
					if (chdir(parse_result[1]) < 0) {
						perror("Failed to change directory: ");
					}
					nullify_pointer_array(parse_result, ARG_NUM);
					cmd_index++;
					continue;
				}
			}

			int status = 0;
			// Parse single command in child process.
			int pid = fork();
			if (pid == 0) {

				// Even cmd and has next cmd, then redirect stdout.
				if (cmd_index % 2 == 0 && split_result[cmd_index + 1] != NULL) {
					// Write to pipe fd_even_to_odd.
					close(fd_even_to_odd[0]);
					dup2(fd_even_to_odd[1], STDOUT_FILENO);
					close(fd_even_to_odd[1]);
				}
				// Even cmd has previous cmd, then redirect stdin.
				if (cmd_index % 2 == 0 && cmd_index > 0) {
					// Read from odd_to_even.
					close(fd_odd_to_even[1]);
					dup2(fd_odd_to_even[0], STDIN_FILENO);
					close(fd_odd_to_even[0]);
				}
				// Odd cmd always has previous cmd.
				if (cmd_index % 2 == 1) {
					// Read from fd_even_to_odd pipe.
					close(fd_even_to_odd[1]);
					dup2(fd_even_to_odd[0], STDIN_FILENO);
					close(fd_even_to_odd[0]);
					if (split_result[cmd_index + 1] != NULL) {
						// Write to fd_odd_to_even pipe.
						close(fd_odd_to_even[0]);
						dup2(fd_odd_to_even[1], STDOUT_FILENO);
						close(fd_odd_to_even[1]);
					}
				}

				// Execution
				if (builtin(parse_result, cmd) != 0) {
					execvp(parse_result[0], parse_result);
					// Command not found will set errno to 2.
					if (errno == 2) {
						// Print error message to stderr.
						fprintf(
						    stderr,
						    "Command( %s ) not found--Did you mean something "
						    "else?\n",
						    parse_result[0]);
					} else {
						// If the error is not "Command not found", just print
						// the error message from system.
						perror("Error:");
					}
					exit(-1);
				} else {
					// End processes.
					exit(0);
				}

			} else {
				// Wait for the child.
				wait(&status);
				// After first child run. close write end of the pipe.
				if (cmd_index % 2 == 0 && split_result[cmd_index + 1] != NULL) {
					if (split_result[cmd_index + 2] != NULL) {
						// After cmd 2, 4, 6 ...
						// That is before cmd 3, 5, 7 ...
						// Before cmd 0, build pipe 0-1,
						// Before cmd 1, build pipe 1-2,

						if (cmd_index > 0) {
							close(fd_odd_to_even[0]);
						}
						pipe(fd_odd_to_even);
					}
					close(fd_even_to_odd[1]);
				}

				if (cmd_index % 2 == 1 && split_result[cmd_index + 1] != NULL) {
					if (split_result[cmd_index + 2] != NULL) {
						// After cmd 1, 3, 5 ...
						// That is before cmd 2, 4, 6 ...
						// Before cmd 0, build pipe 0-1,
						// Before cmd 2, build pipe 2-3,
						close(fd_even_to_odd[0]);
						pipe(fd_even_to_odd);
					}
					close(fd_odd_to_even[1]);
				}
				// Close all the pipe ends in the last loop.
				if (split_result[cmd_index + 1] == NULL) {
					if (cmd_index > 0) {
						// Write end has been closed before.
						close(fd_even_to_odd[0]);
					}
					if (cmd_index > 1) {
						close(fd_odd_to_even[0]);
					}
				}
				// free parser results.
				nullify_pointer_array(parse_result, ARG_NUM);
			}
			cmd_index++;
		}
		// free split commands.
		nullify_pointer_array(split_result, ARG_NUM);
	}
}
