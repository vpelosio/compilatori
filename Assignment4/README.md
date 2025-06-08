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
chmod +x runTests.sh
./runTests.sh
```
