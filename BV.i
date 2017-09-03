%module BV
%{
#include "lib/he/BV_basic.h"
%}
extern void pari_init(size_t parisize, int maxprime);
extern void pari_close();
typedef long *GEN;
%include "lib/he/BV_basic.h"
%include "lib/he/BV_keys.h"
%include "lib/he/BV_utils.h"
