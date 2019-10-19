#!/bin/bash

rm -rf .depend

for f in *.cc
do
  clang++ -std=c++17 -MM $f -MT obj/${f%.cc}.o >> .depend
  echo -e "\t\$(CC) \$(CPPFLAGS) \$< -o \$@" >> .depend
  echo >> .depend
done

for f in tests/*.cc
do
  clang++ -std=c++17 -MM $f -MT obj/${f%.cc}.o >> .depend
  echo -e "\t\$(CC) \$(CPPFLAGS) \$< -o \$@" >> .depend
  echo >> .depend
done
