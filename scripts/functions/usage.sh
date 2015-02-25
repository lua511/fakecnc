#!/bin/bash

usage(){
    P=${0##*/}
    cat <<EOF
$P: Run LINUXCNC
Usage:
    $P [-d] [-v]
         Choose the configuration file graphically

    $P [-k] [-d] [-v] path/to/your.ini
         Name the configuration file using its path

    $P [-k] [-d] [-v] -l
        Use the previous configuration file

    -d: Turn on "debug" mode
    -v: Turn on "verbose" mode
        -k: Continue in the presence of errors in .hal files
EOF
}

usage
