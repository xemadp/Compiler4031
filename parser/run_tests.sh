#!/bin/bash

# Directory containing test cases
TEST_DIR="testcases"

# Iterate over input files in the testcases directory
for input_file in "$TEST_DIR"/input*.txt; do
    # Extract the number from the input file name
    num=$(basename "$input_file" | grep -o '[0-9]\+')

    # Copy input file to working directory as input.txt
    cp "$input_file" input.txt

    # Run the parser
    ./parser

    # Save the output to testcases/output{$num}.txt
    mv output.txt "$TEST_DIR/output${num}.txt"

    echo "Processed $input_file -> output${num}.txt"
done

echo "All test cases processed."
