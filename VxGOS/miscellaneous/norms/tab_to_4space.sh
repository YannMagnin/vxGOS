#!/usr/bin/env bash

#TODO : check if git status is empty
#TODO : for loop trough args


# check args
[[ $# == 0 ]] && echo 'missing source prefix list' >&2 && exit 1

base_prefix='/tmp/vxkernel/tab-to-4space/'

mkdir -p "$base_prefix"

for arg in "$@"; do
    for file_src in $(find "$arg" -name '*\.[cSh]'); do
        echo "update file $file_src..."
        file_dst="$base_prefix$(basename $file_src)"
        expand -t 4 "$file_src" > "$file_dst"
        mv "$file_dst" "$file_src"
    done
done
