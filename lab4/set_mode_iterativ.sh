#! /bin/sh

if test $# -ne 2 
then 
    echo "Numar argumente incorect"
    exit 
fi

director=$1
caracter=$2

for file in "$director"/*.txt
do
    if test "$caracter" == "x"
    then
        chmod +x "$file"
    fi
    
    if test "$caracter" == "r"
    then
        chmod +r "$file"
    fi

    if test "$caracter" == "w"
    then 
        chmod +w "$file"
    fi
done
