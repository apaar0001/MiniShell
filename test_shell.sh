#!/bin/bash

# Colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Store the path to minishell
MINISHELL_PATH="../minishell"

# Function to check if minishell exists and compile if needed
setup_shell() {
    echo -e "${YELLOW}Checking minishell executable...${NC}"
    
    # Check if minishell exists
    if [ ! -f "$MINISHELL_PATH" ]; then
        echo -e "${YELLOW}Compiling minishell...${NC}"
        cd ..
        gcc -o minishell src/main.c src/parser/parser.c src/builtins/builtins.c src/core/env.c src/readline/readline.c src/readline/history.c src/utils/string_utils.c src/core/execute.c -I include -lreadline
        
        if [ $? -ne 0 ]; then
            echo -e "${RED}Failed to compile minishell${NC}"
            exit 1
        fi
        echo -e "${GREEN}Compilation successful${NC}"
        cd test_files
    else
        echo -e "${GREEN}minishell executable found${NC}"
    fi
    
    # Make sure minishell is executable
    chmod +x "$MINISHELL_PATH"
}

# Function to run a test
run_test() {
    local test_name="$1"
    local command="$2"
    local expected_output="$3"
    local expected_status="$4"

    echo -e "\n${GREEN}Running test: ${test_name}${NC}"
    echo "Command: $command"
    
    # Run the command and capture output and status
    output=$(printf "%s\nexit\n" "$command" | "$MINISHELL_PATH" 2>&1)
    status=$?
    
    # Check status
    if [ $status -eq $expected_status ]; then
        echo -e "${GREEN}✓ Status check passed${NC}"
    else
        echo -e "${RED}✗ Status check failed${NC}"
        echo "Expected status: $expected_status"
        echo "Got status: $status"
    fi
    
    # Check output if expected_output is provided
    if [ ! -z "$expected_output" ]; then
        if [ "$output" = "$expected_output" ]; then
            echo -e "${GREEN}✓ Output check passed${NC}"
        else
            echo -e "${RED}✗ Output check failed${NC}"
            echo "Expected output: $expected_output"
            echo "Got output: $output"
        fi
    fi
}

# Function to run a test with file redirection
run_test_with_file() {
    local test_name="$1"
    local command="$2"
    local expected_file="$3"
    local expected_content="$4"
    local expected_status="$5"

    echo -e "\n${GREEN}Running test: ${test_name}${NC}"
    echo "Command: $command"
    
    # Run the command
    printf "%s\nexit\n" "$command" | "$MINISHELL_PATH"
    status=$?
    
    # Check status
    if [ $status -eq $expected_status ]; then
        echo -e "${GREEN}✓ Status check passed${NC}"
    else
        echo -e "${RED}✗ Status check failed${NC}"
        echo "Expected status: $expected_status"
        echo "Got status: $status"
    fi
    
    # Check file content
    if [ -f "$expected_file" ]; then
        content=$(cat "$expected_file")
        if [ "$content" = "$expected_content" ]; then
            echo -e "${GREEN}✓ File content check passed${NC}"
        else
            echo -e "${RED}✗ File content check failed${NC}"
            echo "Expected content: $expected_content"
            echo "Got content: $content"
        fi
        rm "$expected_file"
    else
        echo -e "${RED}✗ File not created${NC}"
    fi
}

# Function to run a test with pipe
run_test_with_pipe() {
    local test_name="$1"
    local command="$2"
    local expected_output="$3"
    local expected_status="$4"

    echo -e "\n${GREEN}Running test: ${test_name}${NC}"
    echo "Command: $command"
    
    # Run the command and capture output and status
    output=$(printf "%s\nexit\n" "$command" | "$MINISHELL_PATH" 2>&1)
    status=$?
    
    # Check status
    if [ $status -eq $expected_status ]; then
        echo -e "${GREEN}✓ Status check passed${NC}"
    else
        echo -e "${RED}✗ Status check failed${NC}"
        echo "Expected status: $expected_status"
        echo "Got status: $status"
    fi
    
    # Check output
    if [ "$output" = "$expected_output" ]; then
        echo -e "${GREEN}✓ Output check passed${NC}"
    else
        echo -e "${RED}✗ Output check failed${NC}"
        echo "Expected output: $expected_output"
        echo "Got output: $output"
    fi
}

# Function to run history tests
run_history_test() {
    local test_name="$1"
    local commands="$2"
    local expected_output="$3"
    local expected_status="$4"

    echo -e "\n${GREEN}Running test: ${test_name}${NC}"
    echo "Commands: $commands"
    
    # Run the commands and capture output
    output=$(printf "%s\nexit\n" "$commands" | "$MINISHELL_PATH" 2>&1)
    status=$?
    
    # Check status
    if [ $status -eq $expected_status ]; then
        echo -e "${GREEN}✓ Status check passed${NC}"
    else
        echo -e "${RED}✗ Status check failed${NC}"
        echo "Expected status: $expected_status"
        echo "Got status: $status"
    fi
    
    # Check output
    if [ "$output" = "$expected_output" ]; then
        echo -e "${GREEN}✓ Output check passed${NC}"
    else
        echo -e "${RED}✗ Output check failed${NC}"
        echo "Expected output: $expected_output"
        echo "Got output: $output"
    fi
}

# Create a temporary directory for test files
mkdir -p test_files
cd test_files

# Setup the shell before running tests
setup_shell

echo -e "${GREEN}Starting shell tests...${NC}"

# Basic Command Tests
echo -e "\n${GREEN}=== Basic Command Tests ===${NC}"
run_test "Basic command" "echo Hello World" "Hello World" 0
run_test "Command with arguments" "echo Hello World 123" "Hello World 123" 0
run_test "Echo with -n" "echo -n Hello" "Hello" 0
run_test "Empty command" "" "" 0

# Built-in Command Tests
echo -e "\n${GREEN}=== Built-in Command Tests ===${NC}"
run_test "PWD command" "pwd" "$(pwd)" 0
run_test "CD command" "cd .. && pwd" "$(cd .. && pwd)" 0
run_test "CD to non-existent directory" "cd nonexistent_dir" "cd: nonexistent_dir: No such file or directory" 1
run_test "CD without arguments" "cd" "cd: HOME not set" 1

# Environment Variable Tests
echo -e "\n${GREEN}=== Environment Variable Tests ===${NC}"
run_test "Export single variable" "export TEST_VAR=hello && echo \$TEST_VAR" "hello" 0
run_test "Export multiple variables" "export A=1 B=2 C=3 && echo \$A \$B \$C" "1 2 3" 0
run_test "Export without value" "export TEST_VAR && echo \$TEST_VAR" "" 0
run_test "Unset variable" "export TEST_VAR=hello && unset TEST_VAR && echo \$TEST_VAR" "" 0
run_test "Unset multiple variables" "export A=1 B=2 && unset A B && echo \$A \$B" " " 0
run_test "ENV command" "env" "$(env)" 0

# History Tests
echo -e "\n${GREEN}=== History Tests ===${NC}"
run_history_test "Basic history" "echo Hello\nhistory" "Hello\n1 echo Hello" 0
run_history_test "Multiple commands" "echo Hello\necho World\nhistory" "Hello\nWorld\n1 echo Hello\n2 echo World" 0
run_history_test "Empty history" "history" "" 0
run_history_test "History with empty lines" "echo Hello\n\necho World\nhistory" "Hello\n\nWorld\n1 echo Hello\n2 \n3 echo World" 0
run_history_test "History with invalid commands" "nonexistent_command\necho Hello\nhistory" "minishell: nonexistent_command: command not found\nHello\n1 nonexistent_command\n2 echo Hello" 0

# File I/O Tests
echo -e "\n${GREEN}=== File I/O Tests ===${NC}"

# Input Redirection Tests
echo "Line 1" > input1.txt
echo "Line 2" >> input1.txt
run_test "Input redirection" "cat < input1.txt" "Line 1\nLine 2" 0
run_test "Input redirection with nonexistent file" "cat < nonexistent.txt" "minishell: nonexistent.txt: No such file or directory" 1

# Output Redirection Tests
run_test_with_file "Output redirection" "echo Hello > output1.txt" "output1.txt" "Hello" 0
run_test_with_file "Output redirection with multiple lines" "echo -e 'Line 1\nLine 2' > output2.txt" "output2.txt" "Line 1\nLine 2" 0
run_test_with_file "Append output redirection" "echo World >> output3.txt" "output3.txt" "Hello\nWorld" 0

# Combined I/O Tests
echo "Input content" > combined.txt
run_test_with_file "Combined input and output" "cat < combined.txt > output4.txt" "output4.txt" "Input content" 0
run_test_with_file "Multiple redirections" "echo Hello > output5.txt && cat < output5.txt > output6.txt" "output6.txt" "Hello" 0

# Pipe Tests
echo -e "\n${GREEN}=== Pipe Tests ===${NC}"
run_test_with_pipe "Simple pipe" "echo Hello | cat" "Hello" 0
run_test_with_pipe "Multiple pipes" "echo Hello | cat | cat" "Hello" 0
run_test_with_pipe "Pipe with grep" "echo -e 'Hello\nWorld' | grep Hello" "Hello" 0
run_test_with_pipe "Pipe with wc" "echo -e 'Hello\nWorld' | wc -l" "2" 0
run_test_with_pipe "Pipe with sort" "echo -e 'B\nA\nC' | sort" "A\nB\nC" 0

# Error Handling Tests
echo -e "\n${GREEN}=== Error Handling Tests ===${NC}"
run_test "Command not found" "nonexistent_command" "minishell: nonexistent_command: command not found" 127
run_test "Invalid command syntax" "echo |" "minishell: syntax error near unexpected token '|'" 2
run_test "Invalid redirection" "echo >" "minishell: syntax error near unexpected token 'newline'" 2
run_test "Invalid pipe" "| echo" "minishell: syntax error near unexpected token '|'" 2
run_test "Invalid input redirection" "cat <" "minishell: syntax error near unexpected token 'newline'" 2
run_test "Invalid output redirection" "echo >" "minishell: syntax error near unexpected token 'newline'" 2

# Special Character Tests
echo -e "\n${GREEN}=== Special Character Tests ===${NC}"
run_test "Command with spaces" "echo 'Hello   World'" "Hello   World" 0
run_test "Command with quotes" "echo \"Hello 'World'\"" "Hello 'World'" 0
run_test "Command with special characters" "echo \$PATH" "$PATH" 0
run_test "Command with escaped characters" "echo Hello\\ World" "Hello World" 0

# Exit Command Tests
echo -e "\n${GREEN}=== Exit Command Tests ===${NC}"
run_test "Exit with status" "exit 42" "" 42
run_test "Exit without status" "exit" "" 0
run_test "Exit with invalid status" "exit abc" "minishell: exit: abc: numeric argument required" 2

# Cleanup
cd ..
rm -rf test_files

echo -e "\n${GREEN}All tests completed!${NC}" 