#ifndef SMILE_NUMARRAY_H
#define SMILE_NUMARRAY_H

// {{SMILE_PUBLIC_HEADER}}

#include <cassert>
#include <cstring>
#include "general.h"
#include "platform.h"

#ifdef DSL_INITLIST_SUPPORTED
#include <initializer_list>
#endif

#define DSL_ASSERT_VALID_NUMARRAY assert(reserved > 0 && size >= 0 && size <= reserved)


template <class T, int LOCAL_SIZE>
class DSL_numArray
{
public:
	DSL_numArray() { Init(); }
	DSL_numArray(int initialSize)
	{
		Init();
		SetSize(initialSize);
	}

	DSL_numArray(const DSL_numArray& other)
	{
		Init();
		SetSize(other.size);
		CopyItems(other.items, items, size);
		DSL_ASSERT_VALID_NUMARRAY;
	}
	
#ifdef DSL_INITLIST_SUPPORTED
	DSL_numArray(std::initializer_list<T> initList)
	{
		Init();
		SetSize((int)initList.size());
		T* p = items;
		for (auto x : initList)
		{
			*p++ = x;
		}
		DSL_ASSERT_VALID_NUMARRAY;
	}

	DSL_numArray& operator=(std::initializer_list<T> initList)
	{
		SetSize((int)initList.size());
		CopyItems(initList.begin(), items, size);
		return *this;
	}

#endif

	~DSL_numArray() 
	{ 
		if (NonLocalMode()) delete[] items; 
	}

	DSL_numArray& operator=(const DSL_numArray& other)
	{
		DSL_ASSERT_VALID_NUMARRAY;
		if (this != &other)
		{
			SetSize(other.size);
			CopyItems(other.items, items, size);
		}
		DSL_ASSERT_VALID_NUMARRAY;
		return *this;
	}

	template <class A>
	void CopyFrom(const std::vector<T, A>& v)
	{
		DSL_ASSERT_VALID_NUMARRAY;
		SetSize((int)v.size());
		if (size > 0)
		{
			CopyItems(&v.front(), items, size);
		}
		DSL_ASSERT_VALID_NUMARRAY;
	}

	template <class A>
	void CopyTo(std::vector<T, A>& v) const
	{
		DSL_ASSERT_VALID_NUMARRAY;
		v.assign(items, items + size);
	}

	T* begin() { return items; }
	T* end() { return items + size; }
	const T* begin() const { return items; }
	const T* end() const { return items + size; }
	const T* cbegin() const { return items; }
	const T* cend() const { return items + size; }

	bool IsEmpty() const { return 0 == size; }
	int GetSize() const { return size; }
	T& operator[](int index) { assert(ValidIndex(index)); return items[index]; }
	T operator[](int index) const { assert(ValidIndex(index)); return items[index]; }
	T& Subscript(int index) { return (*this)[index]; }
	T Subscript(int index) const { return (*this)[index]; }
	T* Items() { return items; }
	const T* Items() const { return items; }

	int FindPosition(T value) const
	{
		DSL_ASSERT_VALID_NUMARRAY;
		for (int i = 0; i < size; i++)
		{
			if (items[i] == value)
			{
				return i;
			}
		}
		return DSL_OUT_OF_RANGE;
	}

	bool Contains(T value) const { return FindPosition(value) >= 0; }

	int SetSize(int newSize)
	{
		DSL_ASSERT_VALID_NUMARRAY;
		if (newSize < 0)
		{
			return DSL_OUT_OF_RANGE;
		}
		if (newSize > reserved)
		{
			Reserve(newSize);
		}
		size = newSize;
		DSL_ASSERT_VALID_NUMARRAY;
		return DSL_OKAY;
	}
	
	int Reserve(int newReserved) 
	{
		DSL_ASSERT_VALID_NUMARRAY;
		if (newReserved < 0)
		{
			return DSL_OUT_OF_RANGE;
		}
		if (newReserved <= reserved)
		{
			return DSL_OKAY;
		}
		// at this point we know we have to allocate
		// because initially reserved is equal to local buffer size
		T* newItems = new T[newReserved];
		CopyItems(items, newItems, size);
		if (NonLocalMode())
		{
			delete[] items;
		}
		items = newItems;
		reserved = newReserved;
		DSL_ASSERT_VALID_NUMARRAY;
		return DSL_OKAY;
	}
	
	void Clear() { SetSize(0); }

	void Add(T value)
	{
		DSL_ASSERT_VALID_NUMARRAY;
		if (size == reserved)
		{
			Reserve(2 * reserved);
		}
		items[size++] = value;
		DSL_ASSERT_VALID_NUMARRAY;
	}

	int AddExclusive(T value)
	{
		if (Contains(value))
		{
			return DSL_OUT_OF_RANGE;
		}

		Add(value);
		return DSL_OKAY;
	}
	
	int Insert(int index, T value)
	{
		DSL_ASSERT_VALID_NUMARRAY;
		if (index == size)
		{
			Add(value);
			return DSL_OKAY;
		}

		if (size == reserved)
		{
			assert(reserved > 0);
			reserved *= 2;
			size++;
			T* newItems = new T[reserved];
			for (int i = 0; i < index; i++)
			{
				newItems[i] = items[i];
			}
			newItems[index] = value;
			for (int i = 1 + index; i < size; i++)
			{
				newItems[i] = items[i - 1];
			}
			if (NonLocalMode())
			{
				delete[]items;
			}
			items = newItems;
		}
		else
		{
			for (int i = size; i > index; i--)
			{
				items[i] = items[i - 1];
			}
			size++;
			items[index] = value;
		}

		DSL_ASSERT_VALID_NUMARRAY;
		return DSL_OKAY;
	}
	
	int Delete(int index)
	{
		DSL_ASSERT_VALID_NUMARRAY;
		if (index < 0 || index >= size)
		{
			return DSL_OUT_OF_RANGE;
		}
		size--;
		for (int i = index; i < size; i++)
		{
			items[i] = items[i + 1];
		}
		DSL_ASSERT_VALID_NUMARRAY;
		return DSL_OKAY;
	}
	
	int DeleteByContent(T value)
	{
		int index = FindPosition(value);
		if (index < 0)
		{
			return index;
		}
		return Delete(index);
	}

	int Assign(const T* src, int count)
	{
		int res = SetSize(count);
		if (DSL_OKAY == res)
		{
			for (int i = 0; i < count; i++)
			{
				items[i] = src[i];
			}
		}
		return res;
	}

	void FillWith(T value)
	{
		DSL_ASSERT_VALID_NUMARRAY;
		for (int i = 0; i < size; i++)
		{
			items[i] = value;
		}
	}

	void Swap(DSL_numArray& other)
	{
		DSL_ASSERT_VALID_NUMARRAY;
		if (NonLocalMode())
		{
			if (other.NonLocalMode())
			{
				// both dynamic
				T* tmp = items;
				items = other.items;
				other.items = tmp;
				SwapScalars(other);
			}
			else
			{
				// we're dynamic, other isn't
				other.SwapLocalWithNonLocal(*this);
			}
		}
		else
		{
			if (other.NonLocalMode())
			{
				// other is dynamic, we're not
				SwapLocalWithNonLocal(other);
			}
			else
			{
				int minSize;
				int maxSize;
				T* copySrc;
				T* copyDst;
				if (size > other.size)
				{
					maxSize = size;
					minSize = other.size;
					copySrc = items;
					copyDst = other.items;
				}
				else
				{
					maxSize = other.size;
					minSize = size;
					copySrc = other.items;
					copyDst = items;
				}
				int i;
				for (i = 0; i < minSize; i++)
				{
					T tmp = localItems[i];
					localItems[i] = other.localItems[i];
					other.localItems[i] = tmp;
				}
				for (; i < maxSize; i++)
				{
					copyDst[i] = copySrc[i];
				}
				IntSwap(size, other.size);
			}
		}
	}

    // BACKWARD COMPATIBILITY ONLY
#ifndef SMILE_NO_V1_COMPATIBILITY
    int NumItems() const { return GetSize(); }
    int IsInList(T x) const { return Contains(x) ? 1 : 0; }
    void UseAsList() {}
#endif 

protected:
	int Permute(const int* sourcePermutation, int permSize)
	{
		DSL_ASSERT_VALID_NUMARRAY;
		if ((permSize != size) || !DSL_isPermutation(sourcePermutation, permSize))
		{
			return DSL_OUT_OF_RANGE;
		}
		DSL_numArray tmp(size);
		for (int i = 0; i < size; i++)
		{
			tmp[i] = (*this)[sourcePermutation[i]];
		}
		Swap(tmp);
		return DSL_OKAY;
	}

private:
	T localItems[LOCAL_SIZE];
	T* items;
	int reserved;
	int size;

	void Init()
	{
		items = localItems;
		size = 0;
		reserved = LOCAL_SIZE;
	}

	bool NonLocalMode() const { return items != localItems; }
	bool ValidIndex(int index) const { return index >= 0 && index < size; }
	
	void SwapScalars(DSL_numArray& other)
	{
		IntSwap(reserved, other.reserved);
		IntSwap(size, other.size);
	}
	
	void SwapLocalWithNonLocal(DSL_numArray& other)
	{
		DSL_ASSERT_VALID_NUMARRAY;

		assert(!NonLocalMode());
		assert(size <= LOCAL_SIZE);
		assert(other.NonLocalMode());

		items = other.items;
		other.items = other.localItems;
		for (int i = 0; i < size; i++)
		{
			other.localItems[i] = localItems[i];
		}
		SwapScalars(other);

		assert(NonLocalMode());
		assert(!other.NonLocalMode());
	}

	static void CopyItems(const T* src, T* dst, int itemCount)
	{
		memcpy(dst, src, itemCount * sizeof(T));
	}

	static inline void IntSwap(int& a, int& b)
	{
		int tmp = a;
		a = b;
		b = tmp;
	}
};

template <class T, int LOCAL_SIZE>
bool operator==(const DSL_numArray<T, LOCAL_SIZE>& lhs, const DSL_numArray<T, LOCAL_SIZE>& rhs)
{
	int lhsSize = lhs.GetSize();
	if (rhs.GetSize() == lhsSize)
	{
		for (int i = 0; i < lhsSize; i++)
		{
			if (lhs[i] != rhs[i])
			{
				return false;
			}
		}
		return true;
	}
	return false;
}

template <class T, int LOCAL_SIZE>
bool operator!=(const DSL_numArray<T, LOCAL_SIZE>& lhs, const DSL_numArray<T, LOCAL_SIZE>& rhs)
{
	return !(lhs == rhs);
}

#endif
