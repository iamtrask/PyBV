import numpy as np
import pickle
import BV



class BVTensor():

    # TODO Remove all secret_key!  Only here to support temporary implementation of reencrypt method! Once reencrypt gets the new algo!
    def __init__(self, public_key, data=None, input_is_decrypted=True, secret_key=None):
        self.encrypted = True

        self.public_key = public_key
        self.secret_key = secret_key

        if(type(data) == np.ndarray and input_is_decrypted):
            self.data = BV.ciphertext(data, self.public_key.pk)
        else:
            self.data = data

    def reencrypt(self, op):
        '''
        Successive multiplications lead to growing errors.  Reencryption "cleans" - stops the growth of the errors.
        Current implementation is just a placeholder
        :return:
        '''
        if self.encrypted is False:
            return

        # TODO Remove this stop-gap!  There will in future be a HE re-encryption! deevashwer has a cunning plan!
        plaintext = op.decrypt(self.private_key)
        result = BV.ciphertext(plaintext, self.private_key)

        return (result)

    def __setitem__(self, key, value):
        self.data[key] = value.data
        return self

    def __getitem__(self, i):
        return BVTensor(self.public_key, self.data[i], False)

    def __add__(self, tensor):
        """Performs element-wise addition between two tensors"""

        if(not isinstance(tensor, BVTensor)):
            # try encrypting it
            tensor = BVTensor(self.public_key, np.array([tensor]).astype('float'), True, self.secret_key)

            return BVTensor(self.public_key, self.data + tensor.data, False, self.secret_key)

        if(type(tensor) == BVTensor):
            tensor = BVTensor(self.public_key, tensor.data, False, self.secret_key)

        return BVTensor(self.public_key, self.data + tensor.data, False, self.secret_key)

    def __sub__(self, tensor):
        """Performs element-wise subtraction between two tensors"""

        if(not isinstance(tensor, BVTensor)):
            # try encrypting it
            tensor = self.public_key.encrypt(tensor)
            return BVTensor(self.public_key, self.data - tensor.data, False)

        if(type(tensor) == BVTensor):
            tensor = BVTensor(self.public_key, tensor.data)

        return BVTensor(self.public_key, self.data - tensor.data, False)

    def __isub__(self, tensor):
        """Performs inline, element-wise subtraction between two tensors"""
        self.data -= tensor.data
        return self

    def __mul__(self, tensor):
        """Performs element-wise multiplication between two tensors"""

        if(isinstance(tensor, BVTensor)):
            if(not tensor.encrypted):
                result = self.data * tensor.data
                result = self.reencrypt(result)
                o = BVTensor(self.public_key, result, False, self.secret_key)
                return o
            else:
                return NotImplemented
        else:
            op = self.data * float(tensor)
            op = self.reencrypt(op)
            return BVTensor(self.public_key, op, False, self.secret_key)

    def __truediv__(self, tensor):
        """Performs element-wise division between two tensors"""

        if(isinstance(tensor, BVTensor)):
            if(not tensor.encrypted):
                result = self.data * (1 / tensor.data)
                o = BVTensor(self.public_key, result, False)
                return o
            else:
                return NotImplemented
        else:
            op = self.data * (1 / float(tensor))
            return BVTensor(self.public_key, op, False)

    def sum(self, dim=None):
        """Returns the sum of all elements in the input array."""
        if not self.encrypted:
            return NotImplemented

        if dim is None:
            return BVTensor(self.public_key, self.data.sum(), False)
        else:
            op = self.data.sum(axis=dim)
            return BVTensor(self.public_key, op, False)

    def dot(self, plaintext_x):
        if(not plaintext_x.encrypted):
            return (self * plaintext_x).sum(plaintext_x.dim() - 1)
        else:
            return NotImplemented

    def __str__(self):
        return "BVTensor: " + str(self.data)

    def __repr__(self):
        return "BVTensor: " + repr(self.data)


class Float():

    # TODO Remove secret key as soon as deevashwer's HE clean/reencrypt algo is implemented
    def __init__(self, public_key, data=None, secret_key=None):
        """Wraps pointer to encrypted Float with an interface that numpy
        can use."""

        self.public_key = public_key
        self.secret_key = secret_key

        if(data is not None):
            self.data = self.public_key.pk.encrypt(data)
        else:
            self.data = None

    def reencypt(self):
        # TODO replace with deevashwer's clever new HE algo
        # "Cleaning" controls the noise that will otherwise grow with successive multiplications
        self.data = self.public_key.pk.encrypt(self.secret_key.sk.decrypt(self.data))

    def decrypt(self, secret_key):
        return secret_key.decrypt(self)

    def __add__(self, y):
        """Adds two encrypted Floats together."""

        out = Float(self.public_key, None)
        out.data = self.data + y.data
        return out

    def __sub__(self, y):
        """Subtracts two encrypted Floats."""

        out = Float(self.public_key, None)
        out.data = self.data - y.data
        return out

    def __mul__(self, y):
        """Multiplies two Floats. y may be encrypted or a simple Float."""

        if(type(y) == type(self)):
            out = Float(self.public_key, None, self.secret_key)
            out.data = self.data * y.data
            out.reencypt()
            return out
        elif(type(y) == int or type(y) == float):
            out = Float(self.public_key, None, self.secret_key)
            out.data = self.data * y
            out.reencypt()
            return out
        else:
            return None

    def __truediv__(self, y):
        """Divides two Floats. y may be encrypted or a simple Float."""

        if(type(y) == type(self)):
            out = Float(self.public_key, None)
            out.data = self.data / y.data
            return out
        elif(type(y) == int):
            out = Float(self.public_key, None)
            out.data = self.data / y
            return out
        else:
            return None

    def __repr__(self):
        """This is kindof a boring/uninformative __repr__"""

        return 'e'

    def __str__(self):
        """This is kindof a boring/uninformative __str__"""

        return 'e'

    def serialize(self):
        return pickle.dumps(self)

    def deserialize(b):
        return pickle.loads(b)
