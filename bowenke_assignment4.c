#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define INPUT_LENGTH 2048
#define MAX_ARGS 512

int prev_exit_status = 0;

struct command_line {
    char *argv[MAX_ARGS + 1];
    int argc;
    char *input_file;
    char *output_file;
    bool is_bg;
};

// ---------------- FUNCTION DEFINITIONS ----------------------
struct command_line *parse_input();
void free_command_line(struct command_line *command);


// ------------------- MAIN PROGRAM ---------------------
int main() {
    struct command_line *curr_command;
    while(true) {
        curr_command = parse_input();

        // blank lines & comments ------------------------
        if (curr_command->argc == 0 || curr_command->argv[0][0] == '#') {
            free_command_line(curr_command);
            continue;
        }
        // exit ------------------------------------------
        if (!strcmp(curr_command->argv[0], "exit")) {
            free_command_line(curr_command);
            exit(0);

        // cd ---------------------------------------------
        } else if (!strcmp(curr_command->argv[0], "cd")) {
            if (curr_command->argc == 1) {
                chdir(getenv("HOME"));
            } else {
                chdir(curr_command->argv[1]);
            }
        // status -----------------------------------------
        } else if (!strcmp(curr_command->argv[0], "status")) {
            if (WIFEXITED(prev_exit_status)) {
                printf("exit value %d\n", WEXITSTATUS(prev_exit_status));
            } else {
                printf("terminated by signam %d\n", WTERMSIG(prev_exit_status));
            }
        // other commands -----------------------------------------
        } else {
            pid_t childPid = fork();

            if (childPid == -1) {
                perror("fork");
            }
            else if (childPid == 0) {
                execvp(curr_command->argv[0], curr_command->argv);
                perror("execvp");
                exit(1);
            } else {
                // store previous exit status after child process finishes
                waitpid(childPid, &prev_exit_status, 0);
                if (WIFSIGNALED(prev_exit_status)) {
                    printf("terminated by signal %d\n", WTERMSIG(prev_exit_status));
                }
            } 
        }
        free_command_line(curr_command);
    }
    return EXIT_SUCCESS;
}


// ---------------------- FUNCTIONS -----------------------------

/* parse_input adapted from code provided by CS 374 instructional team */
struct command_line *parse_input() {
    char input[INPUT_LENGTH];
    struct command_line *curr_command = (struct command_line *) calloc(1, sizeof(struct command_line));

    // Get input
    printf(": ");
    fflush(stdout);
    fgets(input, INPUT_LENGTH, stdin);
    
    // Tokenize the input
    char *token = strtok(input, " \n");
    while(token){
        if(!strcmp(token,"<")){
            curr_command->input_file = strdup(strtok(NULL," \n"));
        } else if(!strcmp(token,">")){
            curr_command->output_file = strdup(strtok(NULL," \n"));
        } else if(!strcmp(token,"&")){
            curr_command->is_bg = true;
        } else{
            curr_command->argv[curr_command->argc++] = strdup(token);
        }
        token=strtok(NULL," \n");
    }
    return curr_command;
}

void free_command_line(struct command_line *command) {
    if (!command) {
        return;
    }
    // free each string in argv
    for (int i=0; i<command->argc; i++) {
        free(command->argv[i]);
    }
    // free input file (if exists)
    if (command->input_file) {
        free(command->input_file);
    }
    // free input file (if exists)
    if (command->output_file) {
        free(command->output_file);
    }
    free(command);
}
