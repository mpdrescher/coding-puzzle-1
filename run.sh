#!/bin/bash

valgrind="valgrind --tool=memcheck --track-origins=yes --leak-check=full"

echo "\
()
([)]
{}
()()" | $valgrind ./partone
