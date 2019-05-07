#!/bin/sh

echo "Compilando..."
gcc server*.c -o server.out -lpthread
echo "Gotcha!"
./server.out
