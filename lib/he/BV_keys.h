#include "BV_utils.h"

class secret_key{
private:
    pari_GEN sk;
    
public:
    parameters* params;
    
    secret_key(){};
    
    secret_key(pari_GEN sk, parameters* params){
        this->sk = sk;
        this->params = params;
    }
    
    void initialize(pari_GEN sk, parameters* params){
        this->sk = sk;
        this->params = params;
    }
    
    pari_GEN decrypt(pari_GEN ct){
        pari_GEN m;
        GEN tmp, tmp1;
        int ct_len = (int) glength(ct.value);
        tmp = compo(ct.value, 1);
        for (int i = 1; i < ct_len; i++) {
            tmp1 = gmul(compo(ct.value, i + 1), powgi(sk.value, stoi(i)));
            tmp = gadd(tmp, tmp1);
        }
        int n = params->n;
        
        //print(tmp);
        tmp = lift(tmp);
        tmp1 = lift(gtovecrev(tmp));
        for (int i = 0; i < n; i++) {
            tmp = gshift(compo(tmp1, i + 1), 1);
            if (gcmp(tmp, params->q.value) == 1)
                gel(tmp1, i + 1) = gsub(compo(tmp1, i + 1), params->q.value);
        }
        m.value = lift(gmodulo(tmp1, params->t.value));
        return m;
    }
    
    void serialize(){
        return;
    }
    
    ~secret_key(){};
};

class public_key{
public:
    pari_GEN pk;
    
    parameters* params;
    
    public_key(){};
    
    public_key(pari_GEN pk, parameters* params){
        this->pk = pk;
        this->params = params;
    }
    
    void initialize(pari_GEN pk, parameters* params){
        this->pk = pk;
        this->params = params;
    }
    
    pari_GEN encrypt(pari_GEN m){
        pari_GEN ct;
        ct.value = cgetg(3, t_VEC);
        pari_GEN u, f, g;
        GEN tmp, M;
        
        u = sample_error_polynomial(params);
        //gerepile(ltop, lbot, NULL);
        f = sample_error_polynomial(params);
        //gerepile(ltop, lbot, NULL);
        g = sample_error_polynomial(params);
        //gerepile(ltop, lbot, NULL);
        
        M = gtovecrev(lift(m.value));
        M = lift(M);
        M = gmodulo(M, params->q.value);
        M = gtopolyrev(M, -1);
        M = gmodulo(M, params->F.value);
        
        tmp = gmul(compo(pk.value, 1), u.value);
        tmp = gadd(gmul(params->t.value, g.value), tmp);
        gel(ct.value, 1) = gadd(tmp, M);
        tmp = gmul(compo(pk.value, 2), u.value);
        tmp = gadd(gmul(params->t.value, f.value), tmp);
        gel(ct.value, 2) = gneg(tmp);
        return ct;
    }
    
    void serialize(){
        
        return;
    }
    
    ~public_key(){};
};

struct key_pair{
    secret_key sk;
    public_key pk;
};

class key_gen{
public:
    key_gen(){};
    
    key_pair generate_key(int n, int Q, int T, int sigma){
        key_pair keys;
        parameters* params = new parameters;
        params->t.value = stoi(T);
        params->sigma = sigma;
        params->n = n;
        params->Q = Q;
        GEN v = cgetg(n + 2, t_VEC);
        gel(v, 1) = stoi(1);
        for (int i = 2; i <= n; i++)
            gel(v, i) = gzero;
        gel(v, n + 1) = stoi(1);
        params->F.value = gtopolyrev(v, -1);
        params->q.value = gshift(stoi(1), params->Q);
        while(true){
            params->q.value = gnextprime(gadd(params->q.value, stoi(1)));
            v = lift(gmodulo(params->q.value, gmul(stoi(2), stoi(n))));
            if(gcmp(v, stoi(1)) == 0)
                break;
        }
        params->t.value = gnextprime(gadd(params->t.value, stoi(1)));
        
        pari_GEN temp = generate_secret_key(params);
        keys.sk.initialize(temp, params);
        temp = generate_public_key(temp, params);
        keys.pk.initialize(temp, params);
        return keys;
    }
    
    /*key_pair deserialize(char* public_key, char* secret_key){
     key_pair keys;
     return keys;
     }*/
};
