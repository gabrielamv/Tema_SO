#! /bin/sh

director_intrare=$1
fisier_iesire=$2

total=0
nrcifre=0;

echo "" > $fisier_iesire 
if test $# -ne 2
then 
    echo "Eroare: numar argumente incorect"
    exit
fi

for fisier_intrare in "$director_intrare"/*.txt
do
    nrcifre=`cat $fisier_intrare | wc -m`
    #nrcifre=`exp $nrcifre - 1`
    total=`expr $total + $nrcifre`
    echo "$fisier_intrare Nr. caractere: $nrcifre" >> $fisier_iesire   
done

echo "Nr. total caractere: $total" >> fisier_iesire

