#!/bin/sh

cd ..

find -name \*.cpp \
  -o -name \*.h \
  -o -name \*.am \
  -o -name \*.txt \
  -o -name \*.xml \
  -o -name \*.mana \
  | sort > manaplus.files
