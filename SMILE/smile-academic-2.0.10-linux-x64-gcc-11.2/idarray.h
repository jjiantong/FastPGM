#ifndef SMILE_IDARRAY_H
#define SMILE_IDARRAY_H

// {{SMILE_PUBLIC_HEADER}}

#include "stringarray.h"

class DSL_idArray : public DSL_stringArray
{
 public:
	DSL_idArray(bool enableEmptyIds = false) : emptyIdsEnabled(enableEmptyIds) {}
	DSL_idArray(const DSL_idArray &likeThisOne);
#ifdef DSL_INITLIST_SUPPORTED
	DSL_idArray(std::initializer_list<const char*> il, bool enableEmptyIds = false) : DSL_stringArray(il), emptyIdsEnabled(enableEmptyIds)
	{
		EnsureValidIds();
	}
#endif

	DSL_idArray& operator=(const DSL_idArray& other);

	bool EmptyIdsEnabled() const { return emptyIdsEnabled; }

	int SetString(int index, const char *value);
	int Add(const char *value);
	int Insert(int index, const char *value);
	int CreateAndAddValidId(const char *prefix = 0, int serialNumber = 0);
	int CreateAndInsertValidId(const char *prefix = 0, int serialNumber = 0, int index = 0);
	int CreateValidId(const char* prefix, int serialNumber, char* buffer) const;
	
	void SetAllEmptyIds(int newSize = -1);

 private:
	bool ValidId(const char *value, int index = -1) const;
	void EnsureValidIds();

	bool emptyIdsEnabled;
};

#endif
