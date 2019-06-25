#!/bin/sh

echo "Compilando..."
gcc server*.c -o server.out -lpthread
echo "Gotcha!"
./server.out $1 $2 $3 $4
