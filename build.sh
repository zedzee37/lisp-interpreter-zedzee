nasm -felf64 main.s -o build/main.o
gcc -no-pie build/main.o -o build/main
