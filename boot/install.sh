#!/bin/sh
#
# SPDX-License-Identifier: GPL-2.0-or-later
#
# Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
#
# Arguments:
#   $1 - kernel version
#   $2 - kernel image file
#   $3 - default install path
#

check () {
    if [ ! -f "$1" ]; then
        echo ""                                                   1>&2
        echo " *** Missing file: $1"                              1>&2
        echo ' *** You need to run "make" before "make install".' 1>&2
        echo ""                                                   1>&2
        exit 1
    fi
}

# Make sure the files actually exist
check "$2"

if [ -f $3/lightcore ]; then
    mv $3/lightcore $3/lightcore.old
fi

cat $2 > $3/lightcore
