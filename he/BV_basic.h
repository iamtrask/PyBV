#include "BV_keys.h"

class ciphertext{
public:
    GEN value;
    int degree;
    public_key* pk;
    
    ciphertext(){};
    
    ~ciphertext(){};
    
    ciphertext(public_key* pk){
        this->degree = 0;
        this->pk = pk;
    }
    
    ciphertext(GEN m, public_key* pk){
        this->degree = 2;
        this->pk = pk;
        this->value = pk->encrypt(m);
    }
    
    void initialize(GEN m, public_key* pk){
        this->degree = 2;
        this->pk = pk;
        this->value = pk->encrypt(m);
    }
    
    void initialize(public_key* pk){
        this->degree = 0;
        this->pk = pk;
    }
    
    ciphertext operator+(ciphertext &ct){
        ciphertext result;
        result.value = addition(this->value, ct.value);
        degree = std::max(this->degree, ct.degree);
        result.pk = this->pk;
        return result;
    }
    
    ciphertext operator*(ciphertext &ct){
        ciphertext result;
        result.value = multiplication(this->value, ct.value);
        degree = this->degree + ct.degree - 1;
        result.pk = this->pk;
        return result;
    }
    
    ciphertext operator-(ciphertext &ct){
        ciphertext result;
        result.value = subtraction(this->value, ct.value);
        degree = std::max(this->degree, ct.degree);
        result.pk = this->pk;
        return result;
    }
    
    GEN decrypt(secret_key sk){
        GEN m = sk.decrypt(this->value);
        return m;
    }
};

