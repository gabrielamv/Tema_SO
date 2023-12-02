# Primim un director și verificăm: -dacă e fișier.txt => grep pe fișier
                                   #-dacă e legătură simbol => 
                                   #dacă e director => apelez recursiv 


#! /bin/sh

if test $# -ne 2
then 
    echo "numar argumente incorect"
    exit
fi

director=$1
output=$2

count=0 #numără legăturile simbol
count2=0

echo "" > $2

for inceput in "$director"/*  # parcurgem directorul și toate fișierele din el, oridecare ar fi ele
do
    #testăm să vedem dacă fișierul curent este de tip .txt
    if test -L "$inceput"
        then 
            count=`expr $count + 1`
    
    elif test -f $inceput
    then
        res=`cat "$inceput" | grep -E  "^0[1-9]|[1-2][0-9]|3[0-1]|\-(0[1-9]|1[0-2])\-[0-9][0-9][0-9][0-9]$"`  # dacă fișierul este de tip .txt, aplicăm grep
        # testăm variabila res
        #echo "$res"
        if test -n "$res"                # dacă res diferit de 0, îl afișăm 
        then
            echo $res >> $2   
        fi
        #verificăm dacă fișierul este legătură simbolică
    elif test -d "$inceput"
        then
        count2=`bash $0 $inceput $2`
        count=`expr $count + $count2`        # la count se adaugă valoarea returnată de bash de la count
    fi

done
echo $count

