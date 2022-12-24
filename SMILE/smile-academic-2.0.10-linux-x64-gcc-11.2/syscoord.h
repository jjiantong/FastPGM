#ifndef SMILE_SYSCOORD_H
#define SMILE_SYSCOORD_H

// {{SMILE_PUBLIC_HEADER}}

#include "dmatrix.h"

class DSL_sysCoordinatesImpl
{
public:
	int& operator[](int index) { return currentPosition[index]; }
	int operator[](int index) const { return currentPosition[index]; }
	const DSL_intArray& Coordinates() const { return currentPosition; }
	int Index() const { return currentIntPosition; }

	void GoFirst();

	void LockCoordinate(int coordIndex);
	void UnlockCoordinate(int coordIndex);
	void UnlockAll();

protected:
	DSL_sysCoordinatesImpl(const DSL_Dmatrix& mtx);

	int GoTo(const DSL_Dmatrix& mtx, int index);
	void GoLast(const DSL_Dmatrix& mtx);
	int GoToCurrentPosition(const DSL_Dmatrix& mtx);
	int Next(const DSL_Dmatrix& mtx);
	int Previous(const DSL_Dmatrix& mtx);

private:
	DSL_intArray currentPosition;
	int currentIntPosition;
	DSL_intArray lockedCoords;
};

template <class M, class V>
class DSL_sysCoordinatesBase : public DSL_sysCoordinatesImpl
{
public:
	DSL_sysCoordinatesBase(M& mtx) : DSL_sysCoordinatesImpl(mtx), link(mtx) {}

	V UncheckedValue()
	{
		return link[Index()];
	}

	int GoTo(int index)
	{
		return DSL_sysCoordinatesImpl::GoTo(link, index);
	}

	void GoLast()
	{
		DSL_sysCoordinatesImpl::GoLast(link);
	}

	int GoToCurrentPosition()
	{
		return DSL_sysCoordinatesImpl::GoToCurrentPosition(link);
	}

	int Next()
	{
		return DSL_sysCoordinatesImpl::Next(link);
	}
	
	int Previous()
	{
		return DSL_sysCoordinatesImpl::Previous(link);
	}

private:
	M& link;
};

typedef DSL_sysCoordinatesBase<const DSL_Dmatrix, double> DSL_sysCoordinates;
typedef DSL_sysCoordinatesBase<DSL_Dmatrix, double&> DSL_writeableSysCoordinates;

#endif 
