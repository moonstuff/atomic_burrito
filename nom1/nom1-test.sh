#!/bin/bash

rm -f events-*
for ext in kct#opts=l#bnum=15000000#map=4294967296#pccap=4294967296; do
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
    echo "zcat ../events.gz | head -n $n | time --verbose ~/dev/atomic_burrito/nom1/nom1 events-$n.$ext &> event-$n.$ext.out"
    zcat ../events.gz | head -n $n | time --verbose ~/dev/atomic_burrito/nom1/nom1 events-$n.$ext &> events-$n.$ext.out
  done
done
