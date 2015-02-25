#!/bin/bash

usage(){
    echo "i am usage"
}

while getopts "dvlhkr" opt
do
    case "$opt" in
    d)
        if tty -s; then
            echo "d:yes"
        else
            echo "d:no"
        fi
        ;;
    v)
        echo "v"
        ;;
    l)
         echo "l";;
    k)
        echo "k";;
    h)
        usage
        exit 0;;
    *)
        usage
        exit 1
    esac
done
