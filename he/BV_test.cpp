#include "BV_basic.h"

#define n 2048
#define Q 61
#define T 2030
#define sigma 8

int test(){
    pari_init(2000000000, 2);
    key_gen keyGen;
    key_pair keys;
    keys = keyGen.generate_key(n, Q, T, sigma);
    public_key pk = keys.pk;
    secret_key sk = keys.sk;
    ciphertext ct[3];
    ct[0].initialize(stoi(5), &pk);
    ct[1].initialize(stoi(6), &pk);
    ct[2] = (ct[1] * ct[0]) + ct[1];
    print_GEN(gel(ct[2].decrypt(sk), 1));
    pari_close();
    return 0;
}
