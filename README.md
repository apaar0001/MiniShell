# MiniShell

A lightweight UNIX-style shell implementation that supports basic shell functionality including command execution, I/O redirection, piping, and built-in commands.

## Features

- Command execution (both built-in and external commands)
- I/O redirection (`>`, `>>`, `<`)
- Command history
- Background process execution
- Signal handling (SIGINT, SIGTSTP)
- Built-in commands:
  - `cd`: Change directory
  - `exit`: Exit the shell
  - `pwd`: Print working directory
  - `echo`: Print arguments
  - `history`: Show command history

## Requirements

- GCC compiler
- GNU Readline library
- Make

## Building

To build the project, simply run:

```bash
make
```

This will create a `bin` directory containing the `minishell` executable.

## Usage

Run the shell:

```bash
./bin/minishell
```

The shell will display a prompt showing the current working directory. You can enter commands just like in a regular shell.

## Examples

```bash
# List files
ls -l

# Change directory
cd /path/to/directory

# Output redirection
ls -l > output.txt

# Input redirection
cat < input.txt

# Background process
sleep 10 &

# Command history
history
```

## Cleanup

To clean up build artifacts:

```bash
make clean
```

## License

This project is open source and available under the MIT License.
