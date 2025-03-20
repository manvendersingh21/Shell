#**Shell Implementation Overview**
**This project is a simple shell that provides basic command execution, including built-in commands and support for external programs. Below is a breakdown of key functions and their roles in the shell:**

**Core Functions**
**int lsh_num_builtins()**
**Purpose:** Returns the count of built-in commands (e.g., cd, help, exit).
**Why?** Helps the shell determine how many internal commands it can process.

**int lsh_cd(char ** args)**
**Purpose:** Changes the current working directory to the one specified in args[1].
**Why?** Implements the cd command for navigation.

**int lsh_help(char ** args)**
**Purpose:** Displays a help message listing all built-in commands and their usage.
**Why?** Provides users with guidance on available shell commands.

**int lsh_exit(char ** args)**
**Purpose:** Signals the shell to terminate by returning 0.
**Why?** Implements the exit command for quitting the shell.

**int lsh_launch(char ** args)**
**Purpose:** Forks a child process to execute an external command using execvp(), with the parent process waiting for it to complete.
**Why?** Enables the shell to run external programs.

**int lsh_execute(char ** args)**
**Purpose:** Determines whether a command is built-in or external. Built-in commands are executed directly, while others are passed to **lsh_launch()**.
**Why?** Serves as the central execution logic of the shell.

**Input Handling**
**char * lsh_read_line(void)**
**Purpose:** Reads a line of input from the user using getline(), removes the trailing newline, and returns the string.
**Why?** Ensures efficient reading of user input without a fixed buffer size.

**char ** lsh_split_line(char * line)** 
**Purpose:** Tokenizes a command string into an array of arguments using delimiters ( \t\r\n\a).
**Why?** Necessary for parsing user input and passing arguments to commands.

**Shell Execution Loop
void lsh_loop(void)**
**Purpose:** Implements the shell’s main execution loop, performing the following steps in each iteration:
**Displays a prompt with the current directory.**
**Reads user input (lsh_read_line()).**

**Checks for special operators (> for redirection, | for piping).**
**Handles redirection/piping if detected; otherwise, executes the command (lsh_execute()).
Why?** Provides the core interactive functionality of the shell.

**Program Entry Point**
**int main(int argc, char ** argv)**
**Purpose:** Initializes the shell and calls lsh_loop() to start execution.
**Why?** Serves as the program’s entry point, as required in C.

**System Calls Used**
The shell utilizes several system calls for process control, file operations, and input handling:

**Process management:** fork(), execvp(), waitpid(), exit()
**Directory operations:** chdir(), getcwd()
**File handling:** open(), close(), dup2()
**Pipes and redirection:** pipe()
**Input reading:** read() (via getline())
**This shell provides a minimal yet functional command-line environment, allowing users to execute both built-in and external commands efficiently.**
