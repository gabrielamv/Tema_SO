#! /bin/sh

#grep

if test $# -ne 1
then   
    echo "Numar argumente incorect!"
    exit
fi

contor=0; #numara propozitii formate corect care contin si caracterul dat ca argument in consola 


while read text
do
    t=`echo "$text" | grep -E "^[A-Z]+[A-Za-z0-9,\.\?! ]+[\.\?!]{1}$" | grep -E -v "(\ si)[\ ]*[,]"`
    if test -n "$t"
    then
        t=`echo "$t"| grep -E "$1"`
        if test -n "$t"
        then
            contor=`expr $contor + 1`
        fi
    fi
done

echo "$contor"

