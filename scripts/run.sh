#!/bin/bash

RED=$'\e[1;31m'
GREEN=$'\e[1;32m'
CLEAR=$'\e[0m'

# First, check if the program is compiled (and compile if not)
if [ ! -d bin ]; then

  echo "$RED Project not compiled. Compiling...$CLEAR"
  make

  if [ -d bin ]; then
    echo "$GREEN Project compiled!$CLEAR"
  else
    echo "$RED Compilation failed. Aborting...$CLEAR"
    exit 1
  fi
else
  echo "$GREEN Project compiled, continuing on with script...$CLEAR"
fi

read -p "Two computer mode (y/n)? " comps

for file in "./books/*"; do
  echo "$(basename $file)"
done

read -p "Please type the name of the book to use: " booktouse

if [ "$comps" == "y" ]; then
  read -p "Use same book for both computers (y/n)? " samebook

  if [ "$samebook" == "n" ]; then
    read -p "Please type the name of the second book to use: " secondbooktouse
  fi
fi

if [ "$comps" == "y" ]; then
  if [ "$samebook" == "y" ]; then
    xboard -fcp "bin/notarookie books/$booktouse" -fd "$(pwd)" -scp "bin/notarookie books/$booktouse" -sd "$(pwd)" &
  else
    xboard -fcp "bin/notarookie books/$booktouse" -fd "$(pwd)" -scp "bin/notarookie books/$secondbooktouse" -sd "$(pwd)" &
  fi
else
  xboard -fcp "bin/notarookie books/$booktouse" -fd "$(pwd)" &
fi

