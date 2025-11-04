#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define INPUT_LENGTH 2048
#define MAX_ARGS 512

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
    }
    free_command_line(curr_command);
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
        } else if(!strcmp(token, "exit")){
            exit(0);
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
