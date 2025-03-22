# Assignments corso compilatori
Repository per assignments della seconda parte del corso di compilatori

## Istruzioni per la build
Dalla root del progetto usare i seguenti comandi  (assumendo che llvm sia installato in /usr/bin/):

```
export LLVM_DIR=/usr/bin/
mkdir build && cd build
cmake -DLT_LLVM_INSTALL_DIR=$LLVM_DIR ..
make
```

## Istruzioni per i test
Dalla cartella di test eseguire i seguenti comandi:

**Algebraic identity**
```
clang-19 -O0 -S -emit-llvm -Xclang -disable-O0-optnone AlgebraicIdentityTest.c -o AlgebraicIdentityTest.ll

opt-19 -S -p mem2reg AlgebraicIdentityTest.ll -o AlgebraicIdentityTest.mem2reg.ll

opt-19 -S -load-pass-plugin ../build/libAssignment1.so -p algebraic-identity AlgebraicIdentityTest.mem2reg.ll -o AlgebraicIdentityTest.mem2reg.optimized.ll
```


