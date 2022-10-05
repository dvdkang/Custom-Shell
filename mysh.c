#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

// This is the maximum number of arguments your shell should handle for one command
#define MAX_ARGS 128

int main(int argc, char** argv) {
  // If there was a command line option passed in, use that file instead of stdin
  if (argc == 2) {
    // Try to open the file
    int new_input = open(argv[1], O_RDONLY);
    if (new_input == -1) {
      // TODO: Execute the command instead of printing it belowfprintf(stderr, "Failed to open input
      // file %s\n", argv[1]);
      exit(1);
    }

    // Now swap this file in and use it as stdin
    if (dup2(new_input, STDIN_FILENO) == -1) {
      fprintf(stderr, "Failed to set new file as input\n");
      exit(2);
    }
  }

  char* line = NULL;        // Pointer that will hold the line we read in
  size_t line_size = 0;     // The number of bytes available in line
  int backgroundCount = 0;  // Keep track of number of background processes

  while (true) {
    // Print the shell prompt
    printf("$ ");

    // Get a line of stdin, storing the string pointer in line
    if (getline(&line, &line_size, stdin) == -1) {
      if (errno == EINVAL) {
        perror("Unable to read command line");
        exit(2);
      } else {
        // Must have been end of file (ctrl+D)
        printf("\nShutting down...\n");

        // Exit the infinite loop
        break;
      }
    }

    line[strlen(line) - 1] = ';';  // Change '\n' to semicolon

    char* s[MAX_ARGS + 1];
    char* temp;
    char* curr = line;

    // this loop runs until we process the exit command
    while (true) {
      int index = 0;
      char* delim_position = strpbrk(curr, "&;");
      char delim;

      if (delim_position == NULL) {
        // when there is only one command left, have the last command ends with semicolon
        delim = ';';
      } else {
        delim = *delim_position;  // save the delimeter
        *delim_position = '\0';   // overwrite delimeter with null terminator
      }

      // record the commands between each semicolon
      while ((temp = strsep(&curr, " ")) != NULL) {
        if (strcmp(temp, "") == 0) {
          continue;
        }
        s[index] = temp;  // save command to s
        index++;
      }

      s[index] = NULL;  // put NULL at the end of the commands array

      if (index == 0) {
        // do nothing
      } else if ((strcmp(s[0], "cd")) == 0) {
        chdir("..");
        // move to the directed directory
      } else if (strlen(s[0]) == 0) {
        // do nothing
      } else if ((strcmp(s[0], "exit")) == 0) {
        return 0;
        // exit the shell
      } else {
        pid_t child = fork();  // fork new process
        if (child < 0) {
          fprintf(stderr, "fork failed\n");
        } else if (child == 0) {
          execvp(s[0], s);  // execute
        } else {
          // if the command is followed by a semicolon, we wait until the previous process finished
          if (delim == ';') {
            int status;
            pid_t rc = waitpid(child, &status, 0);
            if (rc == -1) {
              perror("wait failed");
              exit(EXIT_FAILURE);
            } else {
              printf("[%s exited with status %d]\n", s[0], WEXITSTATUS(status));
            }
          } else if (delim == '&') {
            // if the command is followed by a &, we increase the record of background process
            backgroundCount++;
          }
        }
      }
      // when we reached all foreground processes, start cleaning up background processes
      if (delim_position == NULL) {
        int finished = 0;  // record of finished background process
        int status;
        for (int i = 0; i < backgroundCount; i++) {
          pid_t rc_background = waitpid(-1, &status, WNOHANG);
          if (rc_background < 0) {
            perror("wait() error");
          } else if (rc_background == 0) {
            // do nothing
          } else {
            printf("[background process %d exited with status %d]\n", rc_background,
                   WEXITSTATUS(status));
            finished++;
          }
        }
        backgroundCount -= finished;
        break;
      }

      curr = delim_position + 1;  // move the start of the command line to the next command
    }
  }  // while

  // If we read in at least one line, free this space
  if (line != NULL) {
    free(line);
  }

  return 0;
}  // main
