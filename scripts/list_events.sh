#!/bin/bash
#
#
DIR=.
rg --no-heading                 \
   "define EVENT_.*[0-9]+" $DIR \
    | grep -v scripts           \
    | tr ":" " " | tr -s " "    \
    | cut -d" " -f1,3,4         \
    | mlr --ifs=" " --opprint   \
      label file,event,constant \
      then cut -o -f event,constant,file \
      then sort -n -f constant \
    | less
