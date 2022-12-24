#ifndef DSL_PROGRESS_H
#define DSL_PROGRESS_H

// {{SMILE_PUBLIC_HEADER}}

class DSL_progress
{
public:
	// Return false from Tick to stop the algorithm.
	// After receiving false, the algorithm should
	// quit with DSL_INTERRUPTED as error code.
	// Algorithms which cannot estimate the percentage
	// of the task completed can pass negative percComplete.
	// Derived classes should COPY any content of msg
	// before returning, if they plan to use it (display, log, etc.)
	virtual bool Tick(double percComplete = -1, const char *msg = NULL) = 0;
};

#endif
