
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h> 


#define PATH_MAX 4096
#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"
/*
  Function Declarations for builtin shell commands:
 */
int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);

/*
  List of builtin commands, followed by their corresponding functions.
 */
char *builtin_str[] = {
  "cd",
  "help",
  "exit"
};

int (*builtin_func[]) (char **) = {
  &lsh_cd,
  &lsh_help,
  &lsh_exit
};

int lsh_num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}



/**
   @brief Bultin command: change directory.
   @param args List of args.  args[0] is "cd".  args[1] is the directory.
   @return Always returns 1, to continue executing.
 */
int lsh_cd(char **args)
{
  if (args[1] == NULL) {
    fprintf(stderr, "MS shell: expected argument to \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("lsh");
    }
  }
  return 1;
}

/**
   @brief Builtin command: print help.
   @param args List of args.  Not examined.
   @return Always returns 1, to continue executing.
 */
int lsh_help(char **args)
{
  int i;
  printf("Manvender Singh's MS Shell\n");
  printf("Type program names and arguments, and hit enter.\n");
  printf("The following are built in:\n");

  for (i = 0; i < lsh_num_builtins(); i++) {
    printf("  %s\n", builtin_str[i]);
  }

  printf("Use the man command for information on other programs.\n");
  return 1;
}

/**
   @brief Builtin command: exit.
   @param args List of args.  Not examined.
   @return Always returns 0, to terminate execution.
 */
int lsh_exit(char **args)
{
  return 0;
}

/**
  @brief Launch a program and wait for it to terminate.
  @param args Null terminated list of arguments (including program).
  @return Always returns 1, to continue execution.
 */
int lsh_launch(char **args)
{
  pid_t pid, wpid;
  int status;

  pid = fork();
  if (pid == 0) {
    // Child process
    if (execvp(args[0], args) == -1) {
      perror("MS Shell");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    // Error forking
    perror("MS Shell");
  } else {
    // Parent process
    do {
      wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}

/**
   @brief Execute shell built-in or launch program.
   @param args Null terminated list of arguments.
   @return 1 if the shell should continue running, 0 if it should terminate
 */
int lsh_execute(char **args)
{
  int i;

  if (args[0] == NULL) {
    // An empty command was entered.
    return 1;
  }

  for (i = 0; i < lsh_num_builtins(); i++) {
    if (strcmp(args[0], builtin_str[i]) == 0) {
      return (*builtin_func[i])(args);
    }
  }

  return lsh_launch(args);
}

/**
   @brief Read a line of input from stdin.
   @return The line from stdin.
 */

char *lsh_read_line(void)
{
  char *line = NULL;
  size_t bufsize = 0;
  ssize_t nread = getline(&line, &bufsize, stdin);

  if (nread == -1) {
    if (feof(stdin)) {
      exit(EXIT_SUCCESS);  // We recieved an EOF
    } else  {
      perror("readline");
      exit(EXIT_FAILURE);
    }
  }

  if (line[nread - 1] == '\n') line[nread - 1] = '\0';  // Remove newline
  return line;
}


/**
   @brief Split a line into tokens (very naively).
   @param line The line.
   @return Null-terminated array of tokens.
 */
char **lsh_split_line(char *line)
{
  int bufsize = LSH_TOK_BUFSIZE, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token;

  if (!tokens) {
    fprintf(stderr, "MS Shell: allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, LSH_TOK_DELIM);
  while (token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= bufsize) {
      bufsize += LSH_TOK_BUFSIZE;
      tokens = realloc(tokens, bufsize * sizeof(char*));
      if (!tokens) {
        fprintf(stderr, "MS Shell: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, LSH_TOK_DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}

/**
   @brief Loop getting input and executing it.
 */
void lsh_loop(void)
{
  char *line;
  char **args;
  int status;
  char cwd[PATH_MAX];
  char *pipe_ptr;
  char *redir_ptr;

  do {
    printf("MS Shell:~");
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
      printf("%s> ", cwd);  // Print the current path as the prompt.
  } else {
      perror("getcwd() error");
      exit(EXIT_FAILURE);
  }
 
    fflush(stdout);
    line = lsh_read_line();
    pipe_ptr = strstr(line, "|");
    redir_ptr = strstr(line, ">");
    
     if (redir_ptr != NULL) {
      /**
       * Handle output redirection.
       * We assume the command is of the form:
       *   command ... > filename
       */
      *redir_ptr = '\0';   // Split at '>'
      redir_ptr++;         // Move to start of filename

      // Trim leading spaces on the filename
      while (*redir_ptr == ' ') redir_ptr++;

      // Parse the command
      char **args = lsh_split_line(line);

      // Open the file
      int fd = open(redir_ptr, O_WRONLY | O_CREAT | O_TRUNC, 0644);
      if (fd < 0) {
          perror("open");
          free(args);
          continue;
      }

      pid_t pid = fork();
      if (pid == 0) {
          // Child: redirect stdout to the file
          if (dup2(fd, STDOUT_FILENO) == -1) {
              perror("dup2");
              exit(EXIT_FAILURE);
          }
          close(fd);

          // Execute the command
          execvp(args[0], args);
          perror("execvp");
          exit(EXIT_FAILURE);
      }
      else if (pid > 0) {
          // Parent: close file and wait
          close(fd);
          waitpid(pid, NULL, 0);
      }
      else {
          perror("fork");
      }

      free(args);
  }else if(pipe_ptr!=NULL){
      status =1;
      // Handle a command with a single pipe.
      *pipe_ptr = '\0';    // Terminate the left-hand command.
      pipe_ptr++;          // Move pointer to the right-hand command.
      while (*pipe_ptr == ' ') pipe_ptr++;  // Skip leading spaces.

      // Split the two commands into arguments.
      char **left_args = lsh_split_line(line);
      char **right_args = lsh_split_line(pipe_ptr);

      int fd[2];
      if (pipe(fd) == -1) {  // Create a pipe.
          perror("pipe");
          continue;
      }

      pid_t pid1 = fork();
      if (pid1 == 0) {
          // First child: redirect stdout to pipe write end.
          dup2(fd[1], STDOUT_FILENO);
          close(fd[0]);
          close(fd[1]);
          execvp(left_args[0], left_args);
          perror("execvp");
          exit(EXIT_FAILURE);
      }

      pid_t pid2 = fork();
      if (pid2 == 0) {
          // Second child: redirect stdin from pipe read end.
          dup2(fd[0], STDIN_FILENO);
          close(fd[1]);
          close(fd[0]);
          execvp(right_args[0], right_args);
          perror("execvp");
          exit(EXIT_FAILURE);
      }

      // Parent: close both ends of the pipe and wait for children.
      close(fd[0]);
      close(fd[1]);
      waitpid(pid1, NULL, 0);
      waitpid(pid2, NULL, 0);

      free(left_args);
      free(right_args);
     
    }else{
      args = lsh_split_line(line);
      status = lsh_execute(args);
      free(args);

    }
    free(line);
    
  } while (status);
}

/**
   @brief Main entry point.
   @param argc Argument count.
   @param argv Argument vector.
   @return status code
 */
int main(int argc, char **argv)
{
  // Load config files, if any.

  // Run command loop.
  lsh_loop();

  // Perform any shutdown/cleanup.

  return EXIT_SUCCESS;
}


