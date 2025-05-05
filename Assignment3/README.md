## Istruzioni per la build
Dalla cartella corrente usare i seguenti comandi  (assumendo che llvm sia installato in /usr/bin/):

```
export LLVM_DIR=/usr/bin/
mkdir build && cd build
cmake -DLT_LLVM_INSTALL_DIR=$LLVM_DIR ..
make
```

## Istruzioni per i test
Dalla cartella di test eseguire i seguenti comandi:

```
clang-19 -O0 -S -emit-llvm -Xclang -disable-O0-optnone licm_test.c -o licm_test.ll
opt-19 -S -p mem2reg licm_test.ll -o licm_test.m2r.ll
opt-19 -S -load-pass-plugin ../build/libAssignment3.so -p mylicm licm_test.m2r.ll -o licm_test.m2r.optimized.ll
```
