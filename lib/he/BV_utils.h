#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#define PARI_OLD_NAMES
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pari/pari.h>
//#include <pari.h>
#include <time.h>
#include <string>
#include <sys/time.h>

struct timeval tv;

pari_sp ltop, lbot;

class pari_GEN{
public:
    GEN value;
    
    pari_GEN(){};
    
    pari_GEN(int x){
        value = stoi(x);
        return;
    }
    
    void initialize(GEN x){
        value = x;
        return;
    }
    
    pari_GEN operator+(const pari_GEN GEN_2){
        pari_GEN result;
        result.value = gadd(this->value, GEN_2.value);
        return result;
    }
    
    pari_GEN operator*(const pari_GEN GEN_2){
        pari_GEN result;
        result.value = gmul(this->value, GEN_2.value);
        return result;
    }
    
    pari_GEN operator/(const pari_GEN GEN_2){
        pari_GEN result;
        result.value = gdiv(this->value, GEN_2.value);
        return result;
    }
    
    pari_GEN operator-(const pari_GEN GEN_2){
        pari_GEN result;
        result.value = gsub(this->value, GEN_2.value);
        return result;
    }
    
    pari_GEN operator%(const pari_GEN GEN_2){
        pari_GEN result;
        result.value = gmodulo(this->value, GEN_2.value);
        return result;
    }
    
    bool operator==(const pari_GEN GEN_2){
       if(!gequal(this->value, GEN_2.value))
           return false;
        else
            return true;
    }
};

struct parameters{
    int n, Q, sigma;
    pari_GEN q, t, F;
};

GEN get_element(GEN x, int index){
    /*Function Prototype: GEN get_element(GEN x, int i)
     The function returns the i_th element of x
     */
    return gel(x, index + 1);
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

pari_GEN Sample(int n, double sigma) {
    pari_GEN ret;
    ret.value = cgetg(n + 1, t_VEC);
    double z;
    int i;
    
    for (i = 1; i <= n; i++) {
        z = Gauss(0, sigma);
        z = fabs(round(z)); /*absolute value of Gaussian distribution */
        ret.value[i] = (long) stoi((long) z);
    }
    
    return ret;
}

pari_GEN generate_random(int bit_length){
    gettimeofday(&tv, NULL);
    setrand(stoi(tv.tv_usec + tv.tv_sec*1000000));
    pari_GEN r;
    r.value = randomi(gshift(gen_1, bit_length));
    return r;
}

pari_GEN sample_error_polynomial(parameters* params){
    ltop = avma;
    pari_GEN tmp = Sample(params->n, params->sigma);
    tmp.value = gmodulo(tmp.value, params->q.value);
    tmp.value = gtopolyrev(tmp.value, -1);
    tmp.value = gmodulo(tmp.value, params->F.value);
    lbot = avma;
    return tmp;
}

pari_GEN generate_secret_key(parameters* params){
    pari_GEN sk = sample_error_polynomial(params);
    //gerepile(ltop, lbot, NULL);
    return sk;
}

pari_GEN generate_public_key(pari_GEN sk, parameters* params){
    int i;
    GEN r, tmp, e;
    pari_GEN pk;
    pk.value = cgetg(3, t_VEC);
    int n = params->n;
    
    tmp = cgetg(n + 1, t_VEC);
    for (i = 0; i < n; i++) {
        gel(tmp, i + 1) = generate_random(params->Q).value;
    }
    tmp = gmodulo(tmp, params->q.value);
    r = gtopolyrev(tmp, -1);
    r = gmodulo(r, params->F.value);
    gel(pk.value, 2) = r;
    
    e = sample_error_polynomial(params).value;
    //gerepile(ltop, lbot, NULL);
    
    r = gmul(r, sk.value);
    e = gmul(params->t.value, e);
    gel(pk.value, 1) = gadd(r, e);
    return pk;
}

pari_GEN addition(pari_GEN ct_1, pari_GEN ct_2){
    pari_GEN ct;
    int i, k = (int) glength(ct_1.value), p = (int) glength(ct_2.value);
    
    if (k >= p) {
        ct.value = cgetg(k + 1, t_VEC);
        for (i = 0; i < p; i++)
            gel(ct.value, i + 1) = gadd(compo(ct_1.value, i + 1), compo(ct_2.value, i + 1));
        for (i = p; i < k; i++)
            gel(ct.value, i + 1) = compo(ct_1.value, i + 1);
    } else {
        ct.value = cgetg(p + 1, t_VEC);
        for (i = 0; i < k; i++)
            gel(ct.value, i + 1) = gadd(compo(ct_1.value, i + 1), compo(ct_2.value, i + 1));
        for (i = k; i < p; i++)
            gel(ct.value, i + 1) = compo(ct_2.value, i + 1);
    }
    return ct;
}

pari_GEN subtraction(pari_GEN ct_1, pari_GEN ct_2){
    pari_GEN ct;
    int i, k = (int) glength(ct_1.value), p = (int) glength(ct_2.value);
    
    if (k >= p) {
        ct.value = cgetg(k + 1, t_VEC);
        for (i = 0; i < p; i++)
            gel(ct.value, i + 1) = gsub(compo(ct_1.value, i + 1), compo(ct_2.value, i + 1));
        for (i = p; i < k; i++)
            gel(ct.value, i + 1) = compo(ct_1.value, i + 1);
    } else {
        ct.value = cgetg(p + 1, t_VEC);
        for (i = 0; i < k; i++)
            gel(ct.value, i + 1) = gsub(compo(ct_1.value, i + 1), compo(ct_2.value, i + 1));
        for (i = k; i < p; i++)
            gel(ct.value, i + 1) = compo(ct_2.value, i + 1);
    }
    return ct;
}

pari_GEN multiplication(pari_GEN ct_1, pari_GEN ct_2){
    pari_GEN ct;
    int k = (int) glength(ct_1.value), p = (int) glength(ct_2.value);
    GEN tmp;
    ct.value = cgetg(k + p, t_VEC);
    bool flag[k + p - 1];
    for(int i = 0; i < k + p - 1; i++)
        flag[i] = false;
    
    for(int i = 0; i < k; i++)
        for(int j = 0; j < p; j++){
            //cout << i << " " << j << " " << i + j << endl;
            tmp = gmul(compo(ct_1.value, i + 1), compo(ct_2.value, j + 1));
            if(flag[i + j])
                gel(ct.value, i + j + 1) = gadd(compo(ct.value, i + j + 1), tmp);
            else{
                gel(ct.value, i + j + 1) = tmp;
                flag[i + j] = true;
            }
        }
    return ct;
}

pari_GEN multiplication_pt(pari_GEN ct, pari_GEN pt){
    pari_GEN result = ct * pt;
    return result;
}
