# !/bin/bash

make -C build
sudo cmake --install build
gcc client.c -o client -lscok
gcc server.c -o server -lscok