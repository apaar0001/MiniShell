#!/bin/bash

# Test basic commands
echo "Testing basic commands..."
pwd
echo "Hello, World!"
cd ..
pwd
cd minishell

# Test command chaining and pipes
echo -e "\nTesting command chaining and pipes..."
ls -l | grep "\.c$"
echo "Hello" | tr "a-z" "A-Z"
cat test.sh | wc -l

# Test I/O redirection
echo -e "\nTesting I/O redirection..."
echo "This is a test" > test.txt
cat test.txt
echo "Appending more text" >> test.txt
cat test.txt
cat < test.txt > test2.txt
cat test2.txt

# Test background processes
echo -e "\nTesting background processes..."
sleep 5 &
jobs
sleep 3 &
jobs
fg 1
jobs

# Test built-in commands
echo -e "\nTesting built-in commands..."
cd ..
pwd
cd minishell
echo "Current directory:"
pwd

# Clean up
rm test.txt test2.txt 