%module BV
%{
#include "he/BV_basic.h"
%}
extern void pari_init(size_t parisize, int maxprime);
extern void pari_close();
typedef long *GEN;
%include "he/BV_basic.h"
%include "he/BV_keys.h"
%include "he/BV_utils.h"
