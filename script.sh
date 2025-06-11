#!/bin/bash

#Check if a file exists
if [ -f "./main.c" ] && [ -f "./files/utility.c" ]; then
    echo "file exists"
fi

touch output.txt

echo "Inserire numero di page_frame:"
read page_frame
while [ $page_frame -le 0 ]; do
    echo "Numero di pagine non valide, riprovare"
    read page_frame
done


gcc -pthread ./main.c ./files/utility.c
chmod +x a.out

#Eseguo lo script 2 volte con il rispettivo algoritmo
for i in $(seq 0 1);
do
    ./a.out $page_frame ./inputs/ $i
done

more output.txt

rm output.txt