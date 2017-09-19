%module BV
%{
#include "lib/he/BV_basic.h"
%}
extern void pari_init(size_t parisize, int maxprime);
extern void pari_close();
typedef long *GEN;

%init %{
	pari_init(2000000000, 2);
%}
//%rename(cipher_mult_left) operator*(const ciphertext&, const GEN&);
//%rename(cipher_mult_right) operator*(const GEN&, const ciphertext&);
%include "carrays.i"
%array_class(int, intArray);
%include "lib/he/BV_basic.h"
%include "lib/he/BV_keys.h"
%include "lib/he/BV_utils.h"
%extend pari_GEN{
	char* __str__(){
		return GENtostr(self->value);
	}

	pari_GEN __getitem__(int key){
		pari_GEN result;
		result.value = gel(self->value, key + 1);
		return result;
	} 
};

%extend ciphertext{
	ciphertext(PyObject *int_list, public_key* pk){
        ciphertext* result = new ciphertext();
        int *array = NULL;
        int nInts;
        if (PyList_Check( int_list ))
        {
        		nInts = PyList_Size( int_list );
        		array = (int*) malloc( nInts * sizeof(int) );
        		for ( int ii = 0; ii < nInts; ii++ ){
            			PyObject *oo = PyList_GetItem( int_list, ii );
            			if ( PyInt_Check( oo ) )
                			array[ ii ] = ( int ) PyInt_AsLong( oo );
        		}
        }
        pari_GEN pt;
        pt.value = cgetg(nInts + 1, t_VEC);
        for(int i = 0; i < nInts; i++)
            gel(pt.value, i + 1) = stoi(array[i]);
        result->packing_method(pt, pk);
        return result;
	}

	ciphertext __mul__(const int pt){
		ciphertext result;
		pari_GEN pt_GEN(pt);
		result.value = multiplication_pt(self->value, pt_GEN);
		return result;
	}

	ciphertext __rmul__(const int pt){
		ciphertext result;
		pari_GEN pt_GEN(pt);
		result.value = multiplication_pt(self->value, pt_GEN);
		return result;
	}
};	
