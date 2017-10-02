import pickle

import numpy as np
import syft
from syft import TensorBase
from syft.he.abstract.keys import AbstractKeyPair
from syft.he.abstract.keys import AbstractSecretKey, AbstractPublicKey

import BV
from he.bv.basic import Float, BVTensor


class SecretKey(AbstractSecretKey):

    def __init__(self, sk):
        self.sk = sk

    def decrypt(self, x):
        """Decrypts x. X can be either an encrypted int or a numpy
        vector/matrix/tensor."""

        if(type(x) == Float):
            return self.sk.decrypt(x.data)
        elif(type(x) == TensorBase or type(x) == BVTensor):
            if(x.encrypted):
                return TensorBase(self.decrypt(x.data), encrypted=False)
            else:
                return NotImplemented
        elif(type(x) == np.ndarray):
            sh = x.shape
            x_ = x.reshape(-1)
            out = list()
            for v in x_:
                out.append(self.sk.decrypt(v.data))
            return np.array(out).reshape(sh)
        else:
            return NotImplemented

    def serialize(self):
        return pickle.dumps(self.sk)

    def deserialize(b):
        return SecretKey(pickle.loads(b))


class PublicKey(AbstractPublicKey):

    def __init__(self, pk):
        self.pk = pk

    def zeros(self, dim):
        """Returns an encrypted tensor of zeros"""
        return syft.zeros(dim).encrypt(self)

    def ones(self, dim):
        """Returns an encrypted tensor of ones"""
        return syft.ones(dim).encrypt(self)

    def rand(self, dim):
        """Returns an encrypted tensor with initial numbers sampled from a
        uniform distribution from 0 to 1."""
        return syft.rand(dim).encrypt(self)

    def randn(self, dim):
        """Returns an encrypted tensor with initial numbers sampled from a
        standard normal distribution"""
        return syft.randn(dim).encrypt(self)

    def encrypt(self, x, same_type=False):
        """Encrypts x. X can be either an encrypted int or a numpy
        vector/matrix/tensor."""
        if(type(x) == int or type(x) == float or type(x) == np.float64):
            if(same_type):
                return NotImplemented
            return Float(self, x)
        elif(type(x) == TensorBase):
            if(x.encrypted or same_type):
                return NotImplemented
            return BVTensor(self, x.data)
        elif(type(x) == np.ndarray):
            sh = x.shape
            x_ = x.reshape(-1)
            out = list()
            for v in x_:
                out.append(Float(self, v))
            if(same_type):
                return np.array(out).reshape(sh)
            else:
                return BVTensor(self, np.array(out).reshape(sh))
        else:
            print("format not recognized:" + str(type(x)))
            return NotImplemented

        return self.pk.encrypt(x)

    def serialize(self):
        return pickle.dumps(self.pk)

    def deserialize(b):
        return PublicKey(pickle.loads(b))


class KeyPair(AbstractKeyPair):

    def __init__(self):
        ""

    def deserialize(self, pubkey, seckey):
        self.public_key = PublicKey(pickle.loads(pubkey))
        self.secret_key = SecretKey(pickle.loads(seckey))
        return (self.public_key, self.secret_key)

    def generate(self, n_length=2048, q=61, t=2030, sigma=8):
        '''
        Creates BV key pair
        :param n_length:
        :param q: TODO docmument me!
        :param t: TODO docmument me!
        :param sigma: TODO docmument me!
        :return: Tuple of (public key, secret key)
        '''
        key_generator = BV.key_gen()
        key_pair = key_generator.generate_key(n_length, q, t, sigma)

        self.public_key = PublicKey(key_pair.pk)
        self.secret_key = SecretKey(key_pair.sk)

        return (self.public_key, self.secret_key)

    def sk(self):
        return self.secret_key

    def pk(self):
        return self.public_key
