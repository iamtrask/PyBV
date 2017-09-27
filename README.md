# PyBV
A Python Implementation of the BV Homomorphic Encryption Scheme

The scheme was proposed by Lauter et al. in <a href="https://eprint.iacr.org/2011/405.pdf">Can Homomorphic Encryption be Practical?</a> which was a variant of a homomorphic encryption scheme given by Brakerski et al. in <a href="http://www.wisdom.weizmann.ac.il/~zvikab/localpapers/IdealHom.pdf">Fully Homomorphic Encryption from Ring-LWE and Security for Key Dependent Messages</a>.

The API can be imported using the command "import BV". It currently supports the following functions and classes:


## Installation

### OSX

```
brew cask install xquartz
brew install pari
brew link pari # optional
brew install swig
sh build.sh
```

### Ubuntu

- You'll need to [download and build pari-gp](https://pari.math.u-bordeaux.fr/pub/pari/unix/pari-2.9.3.tar.gz) from source.  Once built, you'll need to add Pari's build output folder to your LD_LIBRARY_PATH so that Pari's .so library can be found when PyBV is built.
- Install swig and build BV.
```
sudo apt-get install swig
sh build.sh
```
- Run `python -m unittest` from within the PyBV folder to verify your installation.

## Troubleshooting

Anaconda users may get a cryptic error relating to *__cxa_throw_bad_array_new_length* when trying to import PyBV due to a conflict between C++ verions. Running the commands mentioned below should fix it.

```
conda remove -y libgcc
sh build.sh
```

## API

### Functions:

#### pari_init(pari_size, max_prime)  

   `pari_init()` is the function that needs to be called before dealing with this API. On importing the module, the module executes this statement automatically. `pari_size` defines the size of stack we'll be using, and `max_prime` defines the pre-computed prime table. By default, `pari_size` is set to 2000000000, and `max_prime` is set to 2.

   _Arguments_: `pari_size (int)`, `max_prime (int)`

#### pari_close()  

   `pari_close()` function has to be called at the end of each program to clear the memory used.


### Classes:

#### pari_GEN

  This class abstracts the `GEN` variable in C++, making it available through a Python interface. The class is compatible with +, \*, /, -, `__getitem__` , %, and `print`.
  * Class Data:
    1. value (`GEN`)

  * `__init__(self, x)`

    The constructor converts `x` to a `GEN` variable.

    _Arguments_: `x(int)`

#### parameters
  * Class Data:
    1. n, Q, sigma (ints)
    2. q, t, F (pari_GEN)

#### secret_key
  * Class Data:
    1. sk (`pari_GEN`)
    2. params (`parameters`)

  * `__init__(self, sk = None, parmas = None)   `
    The constructor initiates class data.

    _Arguments_: `sk (pari_GEN)`, `params (parameters)`

  * `decrypt(self, ct)`   
    `decrypt()` method returns the plaintext (`pari_GEN`) encrypted in ciphertext ct.

    _Arguments_: `ct (pari_GEN)`

  * `serialize(self)`   
    TO BE IMPLEMENTED

#### public_key
  * Class Data:
    1. pk (`pari_GEN`)
    2. params (`parameters*`)

  * `__init__(self, pk = None, params = None)`   
    The constructor initiates the class data.

    _Arguments_: `pk (pari_GEN)`, `params (parameters*)`

  * `encrypt(self, pt)`   
    `encrypt()` method returns the ciphertext (`pari_GEN`) which encrypts plaintext pt.

    _Arguments_: `pt (pari_GEN)`

  * `serialize(self) `  
    TO BE IMPLEMENTED

#### key_pair
  * Class Data:
    1. sk (`secret_key`)
    2. pk (`public_key`)

#### key_gen
  * `generate_key(self, n, Q, T, sigma)`   

    `generate_key()` method returns the keys, which is of type `key_pair`.

    _Arguments_: `n (int)`, `Q (int)`, `T (int)`, `sigma (int)`


  * `deserialize(self)`   
    TO BE IMPLEMENTED

#### ciphertext   

  The class is compatible with '+', '\*', and '-' operators. It also supports multiplication with plaintext (int) and batching.

  * Class Data:
    1. value (`pari_GEN`)
    2. pk (`public_key*`)

  * `__init__(self, plaintext = None, pk)`   

    The plaintext variable is either an `int` variable, or it is an `int_list` variable. The size of list has to be less than the degree of polynomials `(paramter n)` in the polynomial ring.

    _Arguments_: `plaintext (int or int_list), pk (public_key*)`

  * `decrypt(self, sk)`

    `decrypt()` method returns the decrypted ciphertext which is `pari_GEN` variable.

    _Arguments_: `sk (secret_key*)`

--------
