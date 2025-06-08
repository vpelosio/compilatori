#!/bin/bash

# Check if there are any .c files in the current directory
shopt -s nullglob # This option allows the loop to not run if no files match the pattern.

c_files=(*.c) # Create an array of .c files

if [ ${#c_files[@]} -eq 0 ]; then
  echo "No .c test files found"
  exit 0
fi

echo "Running tests..."

# Loop through each .c file
for c_file in "${c_files[@]}"; do
  echo "--------------------------------------------------"
  echo "Running test: $c_file"

  clang-19 -O0 -S -emit-llvm -Xclang -disable-O0-optnone "$c_file"  -o "${c_file%.c}.ll"
  opt-19 -S -p mem2reg "${c_file%.c}.ll" -o "${c_file%.c}.m2r.ll"
  opt-19 -S -load-pass-plugin ../build/libAssignment4.so -p myloopfusion  "${c_file%.c}.m2r.ll" -o "${c_file%.c}.optimized.ll"

  echo "Finished processing $c_file"
done

echo "--------------------------------------------------"
echo "All .c files processed."
