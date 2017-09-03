import BV
from BV import *

pari_init(2000000000, 2)
keys = key_gen()
keys = keys.generate_key(2048, 61, 2030, 8)
ct = ciphertext(create_GEN(19), keys.pk)

