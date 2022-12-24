#ifndef SMILE_DOUBLEARRAY_H
#define SMILE_DOUBLEARRAY_H

// {{SMILE_PUBLIC_HEADER}}

#include "intarray.h"

class DSL_doubleArray : public DSL_numArray<double, 4>
{
public:
	DSL_doubleArray() {}
	DSL_doubleArray(int initialSize) : DSL_numArray(initialSize) {}
	DSL_doubleArray(const DSL_doubleArray& other) : DSL_numArray(other) {}
#ifdef DSL_INITLIST_SUPPORTED
	DSL_doubleArray(std::initializer_list<double> initList) : DSL_numArray(initList) {}
	DSL_doubleArray& operator=(std::initializer_list<double> initList) { DSL_numArray::operator=(initList); return *this; }
#endif

	int ChangeOrder(const DSL_intArray& permutation)
	{
		return Permute(permutation.Items(), permutation.GetSize());
	}

};

#endif
