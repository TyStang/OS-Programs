#include "wish.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_ARGS 100
#define MAX_PATHS 10

char *path_dirs[MAX_PATHS];
int path_count = 0;

void parse_command(char *line, char **args) {
    int i = 0;
    char *token = strtok(line, " \t\n");
    while (token != NULL && i < MAX_ARGS - 1) {
        args[i++] = token;
        token = strtok(NULL, " \t\n");
    }
    args[i] = NULL;
}

void execute_command(char **args) {
    if (strcmp(args[0], "exit") == 0) {
        exit(0);
    }

    if (strcmp(args[0], "cd") == 0) {
        if (args[1] == NULL || chdir(args[1]) != 0) {
            perror("wish");
        }
        return;
    }

    if (strcmp(args[0], "path") == 0) {
        for (int i = 0; i < path_count; i++) {
            printf("%s ", path_dirs[i]);
        }
        printf("\n");
        return;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("wish");
        return;
    }

    if (pid == 0) {
        for (int i = 0; args[i] != NULL; i++) {
            if (strcmp(args[i], ">") == 0) {
                FILE *output_file = fopen(args[i + 1], "w");
                if (!output_file) {
                    perror("wish");
                    exit(EXIT_FAILURE);
                }
                dup2(fileno(output_file), STDOUT_FILENO);
                fclose(output_file);
                args[i] = NULL;
                break;
            }
        }

        for (int i = 0; i < path_count; i++) {
            char full_path[256];
            snprintf(full_path, sizeof(full_path), "%s/%s", path_dirs[i], args[0]);
            if (execvp(full_path, args) != -1) {
                exit(EXIT_SUCCESS);
            }
        }
        perror("wish");
        exit(EXIT_FAILURE);
    } else {
        int status;
        waitpid(pid, &status, 0);
    }
}

FILE *open_input_file(int argc, char *argv[]) {
    if (argc == 2) {
        FILE *input = fopen(argv[1], "r");
        if (!input) {
            perror("wish");
            exit(1);
        }
        return input;
    }
    return stdin;
}

void interactive_mode(int argc, FILE *input) {
    char *line = NULL;
    size_t len = 0;

    while (1) {
        if (argc == 1) {
            printf("wish> ");
        }
        if (getline(&line, &len, input) == -1) {
            break;
        }

        char *args[MAX_ARGS];
        parse_command(line, args);
        if (args[0] != NULL) {
            execute_command(args);
        }
    }

    free(line);
}

void initialize_paths() {
    path_dirs[0] = "/bin";
    path_count = 1;
}

void add_path(char *new_path) {
    if (path_count < MAX_PATHS) {
        path_dirs[path_count++] = new_path;
    } else {
        fprintf(stderr, "Error: Maximum path limit reached.\n");
    }
}

int main(int argc, char *argv[]) {
    if (argc > 2) {
        fprintf(stderr, "Usage: wish [scriptfile]\n");
        exit(1);
    }

    initialize_paths();

    FILE *input = open_input_file(argc, argv);
    interactive_mode(argc, input);

    if (input != stdin) {
        fclose(input);
    }

    return 0;
}
