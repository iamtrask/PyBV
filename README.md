# PyBV
A Python Implementation of the BV Homomorphic Encryption Scheme

The scheme was proposed by Lauter et al. in - Can Homomorphic Encryption be Practical? (https://eprint.iacr.org/2011/405.pdf) which was a variant of the Some-what homomorphic encryption scheme given by Brakerski et al. in - Fully Homomorphic Encryption from Ring-LWE and Security for Key Dependent Messages (http://www.wisdom.weizmann.ac.il/~zvikab/localpapers/IdealHom.pdf).

The API can be imported using the command "import BV". It currently supports the following functions and classes:

## Functions:

### pari_init(pari_size, max_prime)   
   pari_init() is the function that needs to be called before dealing with this API. pari_size defines the size of stack we'll be using, and max_prime defines the pre computed prime table. Arguments: pari_size (int), max_prime (int)

### pari_close()   
   pari_close() function has to be called at the end of each program to clear the memory used.

### create_GEN(x)   
   create_GEN() function converts integer x to GEN

### get_element(x, i)   
   get_element() function returns the i^th element of GEN variable x

### print_GEN(x)   
   print_GEN() function prints the GEN variable x

## Classes:

### parameters
  * Class Data:
    1. n, Q, sigma (ints)
    2. q, t, F (GEN)

### secret_key
  * Class Data:
    1. sk (GEN)
    2. params (parameters)

  * \_\_init\_\_(self, sk = None, parmas = None)   
    The constructor initiates class data. Arguments: sk (GEN), params (parameters)

  * decrypt(self, ct)   
    decrypt() method returns the plaintext (GEN) encrypted in ciphertext ct. Arguments: ct (GEN) 

  * serialize(self)   
    TO BE IMPLEMENTED

### public_key
  * Class Data:
    1. pk (GEN)
    2. params (parameters*)

  * \_\_init\_\_(self, pk = None, params = None)   
    The constructor initiates the class data. Arguments: pk (GEN), params (parameters*)

  * encrypt(self, pt)   
    encrypt() method returns the ciphertext (GEN) which encrypts plaintext pt. Arguments: pt (GEN)

  * serialize(self)   
    TO BE IMPLEMENTED

### key_pair
  * Class Data:
    1. sk (secret_key)
    2. pk (public_key)

### key_gen
  * generate_key(self, n, Q, T, sigma)   
    generate_key() method returns the keys, which is of type key_pair. Arguments: n (int), Q (int), T (int), sigma (int)

  * deserialize(self)   
    TO BE IMPLEMENTED

### ciphertext   
    The class is compatible with '+', '*', and '-' operators
  * Class Data:
    1. value (GEN)
    2. pk (public_key*)

  * \_\_init\_\_(self, plaintext = None, pk)   
    The constuctor method takes two arguments: plaintext (GEN variable), pk (public_key*)

  * decrypt(self, sk)   
    decrypt() method returns the decrypted ciphertext which is GEN variable. Arguments: sk (secret_key*)
