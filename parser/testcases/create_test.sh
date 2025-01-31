#!/bin/bash

# Check if argument is provided
if [ $# -ne 1 ]; then
    echo "Usage: $0 <program_argument>"
    exit 1
fi

# Store the program argument
program_arg="$1"

# Find the last input file number
last_num=0
for file in input*.txt; do
    if [[ $file =~ input([0-9]+)\.txt ]]; then
        num="${BASH_REMATCH[1]}"
        if ((num > last_num)); then
            last_num=$num
        fi
    fi
done

# Calculate new file number
new_num=$((last_num + 1))
new_file="input${new_num}.txt"

# Create the new file with template
cat > "$new_file" << EOF
program ${program_arg};
begin
end
EOF

# Open in default editor
${EDITOR:-vi} "$new_file"
