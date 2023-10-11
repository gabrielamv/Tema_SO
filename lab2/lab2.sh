#! /bin/sh
if test $# -le 3
then
    echo "eroare: numar de argumente incorect"
    exit
fi
fisier=$1
dir=$2
cnt=0
sum=0

shift 2

for arg in "$@"
do 
    if test $arg -gt 10
    then
        cnt=`expr $cnt + 1`
        sum=`expr $sum + $arg`
    fi
done

echo "Numerele mai mari ca 10 sunt: $cnt"

echo "Suma numerelor este: $sum"

nrcifre=`echo $sum | wc -m`
nrcifre=`expr $nrcifre - 1` 

echo "Suma are $nrcifre cifre"

if test -f $fisier
then 

    echo $cnt $sum > $fisier
fi 

fisiere=`ls $dir`
for f in $fisiere
do
    ext=`echo $f | cut -d . -f2`
    if test $ext == "txt"
    then 
        echo "fisierul $f contine:"
        echo `cat $dir/$f`
    fi
done

