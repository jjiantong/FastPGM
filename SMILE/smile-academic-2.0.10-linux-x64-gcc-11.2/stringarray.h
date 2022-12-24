#ifndef SMILE_STRINGARRAY_H
#define SMILE_STRINGARRAY_H

// {{SMILE_PUBLIC_HEADER}}

class DSL_intArray;

#include <vector>
#include <string>
#include "platform.h"
#ifdef DSL_INITLIST_SUPPORTED
#include <initializer_list>
#endif

class DSL_stringArray
{
public:
    DSL_stringArray() {}
    DSL_stringArray(int size) : items(size) {}
    DSL_stringArray(const DSL_stringArray &src);

#ifdef DSL_INITLIST_SUPPORTED
    DSL_stringArray(std::initializer_list<const char*> il)
    {
        CopyPtrVector(il, items);
    }
#endif

    ~DSL_stringArray();

    const char* const* begin() const { return &items.front(); }
    const char* const* end() const { return begin() + items.size(); }

    DSL_stringArray& operator=(const DSL_stringArray &src);
    const char* operator[](int index) const { return items[index]; }
    const char* Subscript(int index) const;
    void Clear();
    void Reserve(int space) { items.reserve(space); }
    bool IsEmpty() const { return items.empty(); }
    int GetSize() const { return (int)items.size(); }
    virtual int SetString(int index, const char *value);
    virtual int Add(const char *value);
    virtual int Insert(int index, const char *value);
    int Delete(int index);
    int DeleteByContent(const char *value);
    int FindPosition(const char *value) const;
    int Contains(const char* value) const { return FindPosition(value) >= 0; }
    int ChangeOrder(const DSL_intArray &sourcePermutation);

#ifndef SMILE_NO_V1_COMPATIBILITY
    // BACKWARD COMPATIBILITY ONLY
    int NumItems() const { return GetSize(); }
#endif

protected:
    bool IndexValid(int index) const;
    static void CopyPtrVector(const std::vector<const char*>& src, std::vector<const char*>& dst);

    std::vector<const char*> items;
};

#endif
