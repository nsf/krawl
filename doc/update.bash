#!/bin/bash

cp ../extra/crawl.lang .
asciidoc -a theme=flask -a toc2 -a icons -a iconsdir=icons crawl_by_example.txt
cp crawl_by_example.html ~/storage/web
