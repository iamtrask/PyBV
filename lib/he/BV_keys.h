#include "BV_utils.h"

class secret_key{
private:
    GEN sk;
    
public:
    parameters* params;
    
    secret_key(){};
    
    secret_key(GEN sk, parameters* params){
        this->sk = sk;
        this->params = params;
    }
    
    void initialize(GEN sk, parameters* params){
        this->sk = sk;
        this->params = params;
    }
    
    GEN decrypt(GEN ct){
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
    
    void serialize(){
        return;
    }
    
    ~secret_key(){};
};

class public_key{
private:
    GEN pk;
    
public:
    parameters* params;
    
    public_key(){};
    
    public_key(GEN pk, parameters* params){
        this->pk = pk;
        this->params = params;
    }
    
    void initialize(GEN pk, parameters* params){
        this->pk = pk;
        this->params = params;
    }
    
    GEN encrypt(GEN m){
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
        params->t = stoi(T);
        params->sigma = sigma;
        params->n = n;
        params->Q = Q;
        GEN v = cgetg(n + 2, t_VEC);
        gel(v, 1) = stoi(1);
        for (int i = 2; i <= n; i++)
            gel(v, i) = gzero;
        gel(v, n + 1) = stoi(1);
        params->F = gtopolyrev(v, -1);
        params->q = gshift(stoi(1), params->Q);
        while(true){
            params->q = gnextprime(gadd(params->q, stoi(1)));
            v = lift(gmodulo(params->q, gmul(stoi(2), stoi(n))));
            if(gcmp(v, stoi(1)) == 0)
                break;
        }
        params->t = gnextprime(gadd(params->t, stoi(1)));
        
        GEN temp = generate_secret_key(params);
        keys.sk.initialize(temp, params);
        temp = generate_public_key(temp, params);
        keys.pk.initialize(temp, params);
        return keys;
    }
    
    /*key_pair deserialize(std::string public_key, std::string secret_key){
        key_pair keys;
        return keys;
    }*/
};
