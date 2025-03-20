# Assignments corso compilatori
Repository per assignments della seconda parte del corso di compilatori

## Build instructions
From the root of the project, use the following commands (assuming llvm is installed in /usr/bin/):

- export LLVM_DIR=/usr/bin/
- mkdir build && cd build
- cmake -DLT_LLVM_INSTALL_DIR=$LLVM_DIR ..
- make

## Test instructions
TODO
