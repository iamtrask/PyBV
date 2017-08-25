/* File : example.i */
%module example

%{
#include "pari/pari.h"
    #include <pari/pari.h>
    using namespace std;
    extern int test();
%}

/* Let's just grab the original header file here */

extern int test();
