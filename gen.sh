#!/bin/bash

for p in $@; do
  while
    hash=$(echo $RANDOM | md5sum | head -c 16)
    data=$(SAKTST=123 ./sak $SAKIP $p $hash /bin/bash)
    [ $? -ne 0 ]
  do true; done
  echo -e "$data"
done
