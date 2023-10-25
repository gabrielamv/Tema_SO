#! /bin/sh

#1) grep -E "^[A-Z]+[A-Za-z0-9, ]+[\.]{1}$"

#2) grep -E -v "(\ si)[\ ]*[,]"

#3) grep -E -v "[n][pb]"

while read text
do
    t=`echo "$text" | grep -E "^[A-Z]+[A-Za-z0-9, ]+[\.]{1}$" | grep -E -v "(\ si)[\ ]*[,]" | grep -E -v "[n][pb]"`
    if test -n "$t"
    then
        echo "$t"
    fi
done < fragment.txt