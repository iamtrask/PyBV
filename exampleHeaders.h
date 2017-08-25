#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#define PARI_OLD_NAMES
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <pari/pari.h>
//#include <pari.h>
#include <time.h>
#include <string.h>
#include <sys/time.h>
#include <vector>

pari_sp ltop, lbot;

struct parameters{
    GEN q, t, F;
    int sigma, n, Q;
};

void print(GEN x){
    std::cout << GENtostr(x) << std::endl;
    return;
}

struct timeval tv;

double Uniform(void) {
    return ((double) rand() + 1.0) / ((double) RAND_MAX + 2.0);
}

double Normal(void) {
    return sqrt(-log(Uniform())*2.0) * sin(2.0 * M_PI * Uniform());
}

double Gauss(double mu, double sigma) {
    double z = sqrt(-2.0 * log(Uniform())) * sin(2.0 * M_PI * Uniform());
    return mu + sigma*z;
}

GEN Sample(int n, double sigma) {
    GEN ret = cgetg(n + 1, t_VEC);
    double z;
    int i;
    
    for (i = 1; i <= n; i++) {
        z = Gauss(0, sigma);
        z = fabs(round(z)); /*absolute value of Gaussian distribution */
        ret[i] = (long) stoi((long) z);
    }
    
    return ret;
}

GEN encode_vector(std::vector<int> input_array){
    int n = input_array.size();
    GEN output_array = cgetg(n + 1, t_VEC);
    for(int i = 0; i < n; i++)
        gel(output_array, i + 1) = stoi(input_array[i]);
    return output_array;
}

GEN generate_random(int bit_length){
    gettimeofday(&tv, NULL);
    setrand(stoi(tv.tv_usec + tv.tv_sec*1000000));
    GEN r = randomi(gshift(gen_1, bit_length));
    return r;
}

GEN sample_error_polynomial(parameters* params){
    ltop = avma;
    GEN tmp = Sample(params->n, params->sigma);
    tmp = gmodulo(tmp, params->q);
    tmp = gtopolyrev(tmp, -1);
    tmp = gmodulo(tmp, params->F);
    lbot = avma;
    return tmp;
}

GEN generate_secret_key(parameters* params){
    GEN sk = sample_error_polynomial(params);
    //gerepile(ltop, lbot, NULL);
    return sk;
}

GEN generate_public_key(GEN sk, parameters* params){
    int i;
    GEN r, tmp, e;
    GEN pk = cgetg(3, t_VEC);
    int n = params->n;
    
    tmp = cgetg(n + 1, t_VEC);
    for (i = 0; i < n; i++) {
        gel(tmp, i + 1) = generate_random(params->Q);
    }
    tmp = gmodulo(tmp, params->q);
    r = gtopolyrev(tmp, -1);
    r = gmodulo(r, params->F);
    gel(pk, 2) = r;
    
    e = sample_error_polynomial(params);
    //gerepile(ltop, lbot, NULL);
    
    r = gmul(r, sk);
    e = gmul(params->t, e);
    gel(pk, 1) = gadd(r, e);
    return pk;
}

GEN encrypt(GEN m, GEN pk, parameters* params) {
    GEN ct = cgetg(3, t_VEC);
    GEN u, f, g;
    GEN tmp, M;
    
    u = sample_error_polynomial(params);
    //gerepile(ltop, lbot, NULL);
    f = sample_error_polynomial(params);
    //gerepile(ltop, lbot, NULL);
    g = sample_error_polynomial(params);
    //gerepile(ltop, lbot, NULL);
    
    M = gtovecrev(lift(m));
    M = lift(M);
    M = gmodulo(M, params->q);
    M = gtopolyrev(M, -1);
    M = gmodulo(M, params->F);
    
    tmp = gmul(compo(pk, 1), u);
    tmp = gadd(gmul(params->t, g), tmp);
    gel(ct, 1) = gadd(tmp, M);
    tmp = gmul(compo(pk, 2), u);
    tmp = gadd(gmul(params->t, f), tmp);
    gel(ct, 2) = gneg(tmp);
    return ct;
}

GEN decrypt(GEN ct, GEN sk, parameters* params) {
    GEN m;
    GEN tmp, tmp1;
    int ct_len = (int) glength(ct);
    tmp = compo(ct, 1);
    for (int i = 1; i < ct_len; i++) {
        tmp1 = gmul(compo(ct, i + 1), powgi(sk, stoi(i)));
        tmp = gadd(tmp, tmp1);
    }
    int n = params->n;
    
    //print(tmp);
    tmp = lift(tmp);
    tmp1 = lift(gtovecrev(tmp));
    for (int i = 0; i < n; i++) {
        tmp = gshift(compo(tmp1, i + 1), 1);
        if (gcmp(tmp, params->q) == 1)
            gel(tmp1, i + 1) = gsub(compo(tmp1, i + 1), params->q);
    }
    m = lift(gmodulo(tmp1, params->t));
    return m;
}

GEN addition(GEN ct_1, GEN ct_2){
    GEN ct;
    int i, k = (int) glength(ct_1), p = (int) glength(ct_2);
    
    if (k >= p) {
        ct = cgetg(k + 1, t_VEC);
        for (i = 0; i < p; i++)
            gel(ct, i + 1) = gadd(compo(ct_1, i + 1), compo(ct_2, i + 1));
        for (i = p; i < k; i++)
            gel(ct, i + 1) = compo(ct_1, i + 1);
    } else {
        ct = cgetg(p + 1, t_VEC);
        for (i = 0; i < k; i++)
            gel(ct, i + 1) = gadd(compo(ct_1, i + 1), compo(ct_2, i + 1));
        for (i = k; i < p; i++)
            gel(ct, i + 1) = compo(ct_2, i + 1);
    }
    return ct;
}

GEN multiplication(GEN ct_1, GEN ct_2){
    GEN ct;
    int k = (int) glength(ct_1), p = (int) glength(ct_2);
    GEN tmp;
    ct = cgetg(k + p, t_VEC);
    bool flag[k + p - 1];
    for(int i = 0; i < k + p - 1; i++)
        flag[i] = false;
    
    for(int i = 0; i < k; i++)
        for(int j = 0; j < p; j++){
            //cout << i << " " << j << " " << i + j << endl;
            tmp = gmul(compo(ct_1, i + 1), compo(ct_2, j + 1));
            if(flag[i + j])
                gel(ct, i + j + 1) = gadd(compo(ct, i + j + 1), tmp);
            else{
                gel(ct, i + j + 1) = tmp;
                flag[i + j] = true;
            }
        }
    return ct;
}
