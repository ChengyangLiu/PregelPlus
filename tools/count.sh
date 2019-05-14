#!/bin/bash
# counting line number of all parted files
FILE=$1
NUM=$2
cnt=0
for ((i=0; i<NUM; i++))
do
  FILENAME=$FILE$i
  tmp=`wc -l $FILENAME`
  tmp=${tmp% *}
  cnt=$[cnt+tmp]
done

echo $cnt
