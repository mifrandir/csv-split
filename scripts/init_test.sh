#!/bin/sh
for D in $(find ./data/test -maxdepth 1 -mindepth 1 -type d); do
  mkdir -p $D/out
done
