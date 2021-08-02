## README

To run: 

make clean && gcc -o main main.c -lpthread -lcurl -ltidy $(pkg-config --libs --cflags libmongoc-1.0) && ./main