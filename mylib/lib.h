/*
#ifndef LIB_H
#define LIB_H
extern void func_signature(int x);

#endif

NOTE: to use this style (define function body in lib.cpp, need to link file in compile command)
*/

#include "stdio.h"

#ifndef LIB_H
#define LIB_H
extern void func_signature(int x) {
    printf("It was declared here :) %d", x);
}

#endif