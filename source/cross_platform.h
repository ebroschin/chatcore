#if defined(_WIN32)
	#define PLATFORM_WINDOWS 1
#else
	#define PLATFORM_WINDOWS 0
#endif

#if PLATFORM_WINDOWS
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif
	#ifndef NOMINMAX
		#define NOMINMAX
	#endif
#endif