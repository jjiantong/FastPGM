#ifndef SMILE_INTARRAY_H
#define SMILE_INTARRAY_H

// {{SMILE_PUBLIC_HEADER}}

#include "numarray.h"

class DSL_intArray : public DSL_numArray<int, 8>
{
public:
	DSL_intArray() {}
	DSL_intArray(int initialSize) : DSL_numArray(initialSize) {}
	DSL_intArray(const DSL_intArray &other) : DSL_numArray(other) {}
#ifdef DSL_INITLIST_SUPPORTED
	DSL_intArray(std::initializer_list<int> initList) : DSL_numArray(initList) {}
	DSL_intArray& operator=(std::initializer_list<int> initList) { DSL_numArray::operator=(initList); return *this; }
#endif

	bool IsPermutation() const;
	bool IsIdentityPermutation() const;
	void MakeIdentityPermutation(int size);

	int ChangeOrder(const DSL_intArray& permutation);
};

#endif
