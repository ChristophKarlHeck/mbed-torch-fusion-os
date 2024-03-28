#!/bin/bash

# Check if the correct number of arguments is provided
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <library_file>"
    exit 1
fi

library_file="$1"

# Check if the library file exists
if [ ! -f "$library_file" ]; then
    echo "Error: Library file '$library_file' not found."
    exit 1
fi

# Extract object files from the static library
ar -t "$library_file" > object_files.txt

# Loop through each object file
while IFS= read -r obj_file; do
    # Extract the corresponding C++ source file
    cpp_file="${obj_file%.obj}"
    path=$(find /home/chris/et-tut-3/mbed-torch-fusion-os -type f -iname "$cpp_file")
    new_path='${CMAKE_CURRENT_SOURCE_DIR}'"${path#*/mbed-torch-fusion-os}"
    # Output the result
    echo "$new_path"
done < object_files.txt

# Remove temporary file
rm object_files.txt
