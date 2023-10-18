#! /bin/sh

if test $# -ne 2 
then 
    echo "Numar argumente incorect"
    exit 
fi

director=$1
caracter=$2

for file in "$director"/*
do
    echo $file
    if test -f $file
    then
        ext=`echo $file | cut -d . -f2` 
        if test $ext == "txt"
        then 
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
        fi
    fi

    if test -d $file 
    then
        ./set_mode_recursiv.sh $file $caracter
    fi
done
