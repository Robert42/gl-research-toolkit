#! /bin/bash

export FILE=files.cmake

echo -e "set(SOURCE_FILES" > $FILE

find . | grep -E '\.(h|inl|cpp)$' | sort -d | sed -E 's/^\.\//  /g' >> $FILE

echo ")" >> $FILE

