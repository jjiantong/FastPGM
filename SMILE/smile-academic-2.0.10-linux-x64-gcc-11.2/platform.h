#ifndef SMILE_PLATFORM_H
#define SMILE_PLATFORM_H

// {{SMILE_PUBLIC_HEADER}}

#ifdef _MSC_VER
	#if _MSC_VER>=1800
		#define DSL_INITLIST_SUPPORTED
	#endif
#else
	#if __cplusplus > 199711L
		#define DSL_INITLIST_SUPPORTED
	#endif
#endif

#endif
