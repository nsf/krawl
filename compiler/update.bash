#!/bin/bash

clang -o lemon/lemon lemon/lemon.c
ragel -o lexer.cpp lexer.rl
./lemon/lemon parser.y
mv parser.h tokens.hpp
mv parser.c parser.cpp
protoc --cpp_out=. brawl.proto
./token-to-string-gen.rb > tokens.cpp
