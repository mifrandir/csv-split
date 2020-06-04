#!/bin/sh
for f in "$@"; do
  if [ -e $f ]; then
    echo $f "- OK"
  else
    echo $f "- MISSING"
    exit 1
  fi
done