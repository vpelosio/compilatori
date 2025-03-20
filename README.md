# Assignments corso compilatori
Repository per assignments della seconda parte del corso di compilatori

## Istruzioni per la build
Dalla root del progetto usare i seguenti comandi  (assumendo che llvm sia installato in /usr/bin/):

- export LLVM_DIR=/usr/bin/
- mkdir build && cd build
- cmake -DLT_LLVM_INSTALL_DIR=$LLVM_DIR ..
- make

## Istruzioni per i test
TODO
