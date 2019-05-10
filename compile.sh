#!/bin/sh

echo "Compilando..."
gcc server*.c -o server.out
echo "Gotcha!"
./server.out $1
