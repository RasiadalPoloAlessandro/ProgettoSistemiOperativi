#!/bin/bash

#Check if a file exists
if test -f ./files/main.c; then
    echo "file exists"
fi

gcc ./main.c ./files/utility.c
chmod +x a.out

./a.out 4
