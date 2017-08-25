#include "exampleHeaders.h"

#define N 2048
#define l 32
#define modulus_size 61
#define T 2030
#define sig 8

using namespace std;

int test(){
    pari_init(2000000000, 2);
    srand(time(NULL));
    int n = N;
    GEN v = cgetg(n + 2, t_VEC);
    parameters* params = new parameters;
    gel(v, 1) = stoi(1);
    for (int i = 2; i <= n; i++)
        gel(v, i) = stoi(0);
    gel(v, n + 1) = stoi(1);
    params->t = stoi(T);
    params->sigma = sig;
    params->n = N;
    params->Q = modulus_size;
    params->F = gtopolyrev(v, -1);
    params->q = gshift(stoi(1), params->Q);
    while(true){
        params->q = gnextprime(gadd(params->q, stoi(1)));
        v = lift(gmodulo(params->q, gmul(stoi(2), stoi(n))));
        if(gcmp(v, stoi(1)) == 0)
            break;
    }
    params->t = gnextprime(gadd(params->t, stoi(1)));
    
    pari_printf("(n, q, t) = (%d, %s, %s)\n", params->n, GENtostr(params->q), GENtostr(params->t));
    printf("Enter the input integers:\n");
    int input_1, input_2;
    scanf("%d %d", &input_1, &input_2);
    printf("Input integers: %d %d\n", input_1, input_2);

    GEN sk, pk, message_1 = stoi(input_1), message_2 = stoi(input_2);

    sk = generate_secret_key(params);
    pk = generate_public_key(sk, params);

    printf("Enter the operation to perform (--|1 for Addition and 2 for Multiplication|--)\n");
    int flag;
    scanf("%d", &flag);
    GEN plaintext;
    if(flag - 1){
        GEN ciphertext_1 = encrypt(message_1, pk, params);
        GEN ciphertext_2 = encrypt(message_2, pk, params);
        GEN result = multiplication(ciphertext_1, ciphertext_2);
        plaintext = decrypt(result, sk, params);

    }
    else{
        GEN ciphertext_1 = encrypt(message_1, pk, params);
        GEN ciphertext_2 = encrypt(message_2, pk, params);
        GEN result = addition(ciphertext_1, ciphertext_2);
        plaintext = decrypt(result, sk, params);
    }
    printf("Decryption result %s\n", GENtostr(gel(plaintext, 1)));
    printf("Cleaning up the Pari stack. Ending program.\n");

    /*vector<int> input_array;
    for(int i = 0; i < n; i++)
        input_array.push_back(1);

    GEN message = encode_vector(input_array);
    GEN ciphertext_1 = encrypt(message, pk, params);
    GEN ciphertext_2 = encrypt(message, pk, params);
    GEN result = multiplication(ciphertext_1, ciphertext_2);
    GEN plaintext = decrypt(result, sk, params);
    print(plaintext);*/
    pari_close();
}
