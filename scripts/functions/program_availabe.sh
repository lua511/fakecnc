#!/bin/bash

program_available () {
    echo "$1"
    type -path "$1" > /dev/null 2>&1
}

if program_available axis-remote ; then
    echo "yes availabe"
else
    echo "not availabe"
fi

if program_available man ; then
    echo "yes man availabe"
else
    echo "no,man is not available"
fi
