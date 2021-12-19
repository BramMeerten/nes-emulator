#!/bin/bash
mkdir -p out
../../build/src/NES > out/actual.txt

# diff --color --width=160 --suppress-common-lines --side-by-side out/actual.txt 01-expected-log.txt | less -10
idea diff out/actual.txt 01-expected-log.txt