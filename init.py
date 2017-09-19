import BV
from BV import *

keys = key_gen()
keys = keys.generate_key(2048, 61, 2030, 8)
ct = ciphertext(19, keys.pk)

