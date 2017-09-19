#include "BV_keys.h"
#include <vector>

class ciphertext{
public:
    int degree;
    pari_GEN value;
    public_key* pk;
    
    ciphertext(){};
    
    ~ciphertext(){};
    
    ciphertext(int m, public_key* pk){
        pari_GEN pt(m);
        this->degree = 2;
        this->pk = pk;
        this->value = pk->encrypt(pt);
    }
    
    void packing_method(pari_GEN m, public_key* pk){
        this->degree = 2;
        this->pk = pk;
        this->value = pk->encrypt(m);
    }
    
    ciphertext operator+(ciphertext &ct){
        ciphertext result;
        result.value = addition(this->value, ct.value);
        result.degree = std::max(this->degree, ct.degree);
        result.pk = this->pk;
        return result;
    }
    
    ciphertext operator*(ciphertext &ct){
        ciphertext result;
        result.value = multiplication(this->value, ct.value);
        result.degree = this->degree + ct.degree - 1;
        result.pk = this->pk;
        return result;
    }
    
    ciphertext operator-(ciphertext &ct){
        ciphertext result;
        result.value = subtraction(this->value, ct.value);
        result.degree = std::max(this->degree, ct.degree);
        result.pk = this->pk;
        return result;
    }
    
    pari_GEN decrypt(secret_key sk){
        pari_GEN m = sk.decrypt(this->value);
        return m;
    }
};
