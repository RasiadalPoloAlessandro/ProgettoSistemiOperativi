#!/bin/bash

#Check if a file exists
if [ -f "./main.c" ] && [ -f "./files/utility.c" ]; then
    echo "file exists"
fi


gcc ./main.c ./files/utility.c
chmod +x a.out

./a.out 4
