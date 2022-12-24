// {{SMILE_PUBLIC_HEADER}}

// SMILE Windows autolink header, do not include directly

#if (_MSC_VER>=1930 && _MSC_VER<1940)
	#define SMILE_MSVC_VER "143"
#elif (_MSC_VER>=1920 && _MSC_VER<1930)
	#define SMILE_MSVC_VER "142"
#elif (_MSC_VER>=1910 && _MSC_VER<1920)
	#define SMILE_MSVC_VER "141"
#elif (_MSC_VER==1900)
	#define SMILE_MSVC_VER "140"
#elif (_MSC_VER==1800)
	#define SMILE_MSVC_VER "120"
#elif (_MSC_VER==1700)
	#define SMILE_MSVC_VER "110"
#elif (_MSC_VER==1600)
	#define SMILE_MSVC_VER "100"
#else
	#error Your version of Visual C++ is not supported
#endif

#ifdef _M_X64
	#define SMILE_ARCH "x64"
#else
	#define SMILE_ARCH "x86"
#endif

#ifdef NDEBUG
	#ifdef _DLL
		#define SMILE_LIBTYPE "_dyn_"
	#else
		#define SMILE_LIBTYPE "_"
	#endif 
#else
	#ifdef _DLL
		#define SMILE_LIBTYPE "_dbg_"
	#else
		#error SMILE : in debug build the runtime library must be set to Multi-threaded Debug DLL 
	#endif
#endif

#define SMILE_LIBNAME "smile" SMILE_LIBTYPE "vc_" SMILE_MSVC_VER SMILE_ARCH
#pragma message("Linking SMILE automatically: " SMILE_LIBNAME)
#pragma comment(lib, SMILE_LIBNAME)