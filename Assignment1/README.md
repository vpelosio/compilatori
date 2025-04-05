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

**Strength reduction**
```
clang-19 -O0 -S -emit-llvm -Xclang -disable-O0-optnone StrengthReductionTest.c -o StrengthReductionTest.ll
opt-19 -S -p mem2reg StrengthReductionTest.ll -o StrengthReductionTest.mem2reg.ll
opt-19 -S -load-pass-plugin ../build/libAssignment1.so -p strength-reduction StrengthReductionTest.mem2reg.ll -o StrengthReductionTest.mem2reg.optimized.ll
```

**Multi-Instruction Optimization**
```
clang-19 -O0 -S -emit-llvm -Xclang -disable-O0-optnone MultiInstOptTest.c -o MultiInstOptTest.ll
opt-19 -S -p mem2reg MultiInstOptTest.ll -o MultiInstOptTest.mem2reg.ll
opt-19 -S -load-pass-plugin ../build/libAssignment1.so -p strength-reduction MultiInstOptTest.mem2reg.ll -o MultiInstOptTest.mem2reg.optimized.ll
```