#!/bin/bash

#Check if a file exists
if [ -f "./main.c" ] && [ -f "./files/utility.c" ]; then
    echo "file exists"
fi

echo "Inserire numero di page_frame:"
read page_frame
echo "Inserire algoritmo di sostituzione: Second Change (1) - FiFo (0)"
read algorithm
while [ $page_frame -le 0 ]; do
    echo "Numero di pagine non valide, riprovare"
    read page_frame
done
while [ $algorithm -ne 0 ] || [ $algorithm -ne 1]; do
    echo "Algoritmo scelto invalido"
    read algorithm
done


gcc ./main.c ./files/utility.c
chmod +x a.out

./a.out $page_frame ./inputs/ $algorithm
