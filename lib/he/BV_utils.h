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

struct timeval tv;

pari_sp ltop, lbot;

struct parameters{
    int n, Q, sigma;
    GEN q, t, F;
};

void print(GEN x){
    std::cout << GENtostr(x) << std::endl;
    return;
}

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

GEN subtraction(GEN ct_1, GEN ct_2){
    GEN ct;
    int i, k = (int) glength(ct_1), p = (int) glength(ct_2);
    
    if (k >= p) {
        ct = cgetg(k + 1, t_VEC);
        for (i = 0; i < p; i++)
            gel(ct, i + 1) = gsub(compo(ct_1, i + 1), compo(ct_2, i + 1));
        for (i = p; i < k; i++)
            gel(ct, i + 1) = compo(ct_1, i + 1);
    } else {
        ct = cgetg(p + 1, t_VEC);
        for (i = 0; i < k; i++)
            gel(ct, i + 1) = gsub(compo(ct_1, i + 1), compo(ct_2, i + 1));
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
