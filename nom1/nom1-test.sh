#!/bin/bash

APPEARANCES_FILE_TREE_DB="appearances.kct"
APPEARANCES_FILE_TREE_DB_TUNED="appearances.kct#opts=l#bnum=15000000#map=4294967296#pccap=4294967296"
PROTO_TREE_DB="+"

#APPEARANCES_DB=$APPEARANCES_FILE_TREE_DB
#URLS_DB=$PROTO_TREE_DB
APPEARANCES_DB=$PROTO_TREE_DB
URLS_DB=$PROTO_TREE_DB

for DB in $PROTO_TREE_DB; do
  for n in \
    00000010 \
    00000100 \
    00001000 \
    00010000 \
    00100000 \
    01000000 \
    10000000 \
    20000000 \
    30000000 \
    40000000;
  do
    echo "Testing $n events with appearances in $APPEARANCES_DB and urls in $URLS_DB"
    zcat ../events.gz | head -n $n | time --verbose ~/dev/atomic_burrito/nom1/nom1 $APPEARANCES_DB $URLS_DB &> $n.out
    [ -f $APPEARANCES_DB ] && mv $APPEARANCES_DB $n-$APPEARANCES_DB
    [ -f $URLS_DB ] && mv $URLS_DB $n-$URLS_DB
  done
done
