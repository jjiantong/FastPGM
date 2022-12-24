#ifndef SMILE_DMATRIX_H
#define SMILE_DMATRIX_H

// {{SMILE_PUBLIC_HEADER}}

#include "intarray.h"
#include "doublearray.h"

class DSL_Dmatrix
{
public:
	DSL_Dmatrix() {}
	DSL_Dmatrix(const DSL_intArray& dims);
	DSL_Dmatrix(const DSL_Dmatrix &src) :
		theDimensions(src.theDimensions),
		thePreProduct(src.thePreProduct),
		theData(src.theData)
	{
	}

#ifdef DSL_INITLIST_SUPPORTED
	DSL_Dmatrix(std::initializer_list<int> dims) { Setup(dims.begin(), (int)dims.size()); };
	int FillFrom(std::initializer_list<double> src) 
	{ 
		if (src.size() != GetSize())
		{
			return DSL_INVALID_VALUE;
		}

		FillFrom(src.begin()); 
		return DSL_OKAY;
	}
#endif

	DSL_Dmatrix& operator=(const DSL_Dmatrix &src)
	{
		if (this != &src)
		{
			theDimensions = src.theDimensions;
			thePreProduct = src.thePreProduct;
			theData = src.theData;
		}
		return *this;
	}

	double &operator[](int index) { return theData[index]; }
	double operator[](int index) const { return theData[index]; }
	double &operator[](const DSL_intArray &theCoordinates);
	double operator[](const DSL_intArray &theCoordinates) const;
	double &operator[] (const int * const * indC) { return theData[CoordinatesToIndex(indC)]; }
	double operator[] (const int * const * indC) const { return theData[CoordinatesToIndex(indC)]; }

	double &Subscript(int index);
	double Subscript(int index) const;
	double &Subscript(const DSL_intArray &coords);
	double Subscript(const DSL_intArray &coords) const;

	double* begin() { return theData.begin(); }
	double* end() { return theData.end(); }
	const double* begin() const { return theData.begin(); }
	const double* end() const { return theData.end(); }
	const double* cbegin() const { return theData.cbegin(); }
	const double* cend() const { return theData.cend(); }

	bool IsEmpty() const { return theDimensions.IsEmpty(); }
	int GetSize() const { return theData.GetSize(); }
	int GetNumberOfDimensions() const { return theDimensions.GetSize(); } 
	int GetLastDimension() const { return GetNumberOfDimensions() - 1 ; }
	int GetSizeOfDimension(int aDimension) const;
	int GetSizeOfLastDimension() const { return GetSizeOfDimension(GetLastDimension()); }

	int Sum(const DSL_Dmatrix &aDmatrix, const DSL_Dmatrix &withDmatrix);
	int Subtract(const DSL_Dmatrix &aDmatrix, const DSL_Dmatrix &minusThisDmatrix);
	int Add(const DSL_Dmatrix &byTheseValues);
	int Add(double byThisMuch);
	int Multiply(double byThisNumber);
	int Multiply(const DSL_doubleArray &byThisVector);
	void FillWith(double value);
	int FillFrom(const DSL_Dmatrix& src);
	int FillFrom(const DSL_doubleArray& src);
	int FillFrom(const std::vector<double>& src);
	void FillFrom(const double* src);

	int Setup(const DSL_intArray& theseDimensions);
	int Setup(const int* dimensions, int dimCount);
	int SetSingleDimension(int dimSize);
	int AddDimension(int thisSize);
	int AddDimensions(const DSL_intArray &newDimensions);
	int InsertDimension(int here, int thisSize);
	int RemoveDimension(int thisDimension);                                    // with nothing
	int RemoveDimension(int thisDimension, int preservingThis);                // with evidence
	int RemoveDimension(int thisDimension, const DSL_doubleArray &usingTheseValues); // with marginals
	int RemoveDimension(int thisDimension, int preservingThis, DSL_Dmatrix &intoThisMatrix) const ;
	int RemoveDimension(int thisDimension, const DSL_doubleArray &usingTheseValues, DSL_Dmatrix &intoThisMatrix) const;
	int RemoveDimensions(const DSL_intArray &theseDimensions, DSL_Dmatrix &intoThisMatrix) const;
	int RemoveDimension(int thisDimension, const DSL_Dmatrix &usingTheseValues, const DSL_intArray &dimensionMapping);

	int IncreaseDimensionSize(int thisDimension);
	int DecreaseDimensionSize(int thisDimension);
	int ResizeDimension(int thisDimension, int thisSize);
	int ShiftDataUpwards(int thisDimension, int fromThisValue, double defVal);
	int ShiftDataUpwards(int thisDim, int fromThisVal) { return ShiftDataUpwards(thisDim, fromThisVal, 0); }
	int ShiftDataDownwards(int thisDimension, int fromThisValue);

	int Normalize(); 
	int Normalize(DSL_intArray &theseCoordinates);
	int Normalize(int thisElement);
	int Normalized(DSL_intArray &theseCoordinates) const;
	int Normalized(int thisElement) const;
	int Complement(DSL_intArray &theseCoordinates);
	int Complement(int thisElement);

	// this CheckElements will substitute the old one (the one below it)
	int CheckElements(double aValue, const DSL_intArray &theseCoordinates) const;
	int CheckElements(double aValue, int firstDim, int firstVal, int secondDim, int secondVal) const;

	void Clear();
  
	void Swap(DSL_Dmatrix &that) { theDimensions.Swap(that.theDimensions); thePreProduct.Swap(that.thePreProduct); theData.Swap(that.theData); }

	int IndexToCoordinates(int theIndex, DSL_intArray &coords) const;
	int CoordinatesToIndex(const int *coords) const;
	int CoordinatesToIndex(const DSL_intArray &coords) const;
	int CoordinatesToIndex(const int * const * indirectCoordinates) const;
	int NextCoordinates(DSL_intArray &coords) const;
	int PrevCoordinates(DSL_intArray &coords) const;

	DSL_doubleArray& GetItems() { return theData; }
	const DSL_doubleArray& GetItems() const { return theData; }
	const DSL_intArray& GetDimensions() const { return theDimensions; }
	const DSL_intArray& GetPreProduct() const { return thePreProduct; }

	int ChangeOrderOfDimensions(const DSL_intArray &newOrder);
	int ChangeOrderWithinDimension(int thisDim, const DSL_intArray &newOrder);

	bool CompatibleWith(const DSL_Dmatrix &other) const;

	const double* Ptr(int idx) const { assert(idx >= 0 && idx < GetSize()); return theData.Items() + idx; }
	const double* Ptr(const DSL_intArray& coords) const { return theData.Items() + CoordinatesToIndex(coords); }

	int CopyBlockFrom(const DSL_Dmatrix& src, int srcIndex, int dstIndex, int blkSize);
	int CopyColumnsFrom(const DSL_Dmatrix& src, int srcColIdx, int dstColIdx, int colCount = 1);

#ifndef SMILE_NO_V1_COMPATIBILITY
	// BACKWARD COMPATIBILITY ONLY
	void CleanUp() { Clear(); }
#endif

protected:
	bool Ok(int index) const { return index >= 0 && index < theData.GetSize(); }
	bool Ok(const DSL_intArray &coords) const;
	int ReCreate(const DSL_intArray &dims) { return ReCreate(dims.Items(), dims.GetSize()); }
	int ReCreate(const int * dims, int dimCount);
	void CalculatePreProd();
	int RemoveDimensionImpl1(int thisDimension, int preservingThis, DSL_intArray &targetDims, DSL_doubleArray &targetData) const;
	int RemoveDimensionImpl2(int thisDimension, const DSL_doubleArray &usingTheseValues, DSL_intArray &targetDims, DSL_doubleArray &targetData) const;

	DSL_intArray    theDimensions;
	DSL_intArray    thePreProduct; // precalculated products of dimensions
	DSL_doubleArray theData;
};

bool operator==(const DSL_Dmatrix& lhs, const DSL_Dmatrix& rhs);
bool operator!=(const DSL_Dmatrix& lhs, const DSL_Dmatrix& rhs);

#endif 
