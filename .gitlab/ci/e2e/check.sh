#!/bin/bash

file_list=$(ls *.yml)

for f in $file_list; do
    retina-request-checker --input $f
done

