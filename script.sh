#!/bin/bash

#Check if a file exists
if test -f ./files/main.c; then
    echo "file exists"
fi

gcc ./files/main.c
chmod +x a.out

./a.out 4
