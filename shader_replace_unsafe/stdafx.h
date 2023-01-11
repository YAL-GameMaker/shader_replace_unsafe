// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifdef _WINDOWS
	#include "targetver.h"
	
	#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
	#include <windows.h>
#endif

#if defined(WIN32)
#define dllx extern "C" __declspec(dllexport)
#elif defined(GNUC)
#define dllx extern "C" __attribute__ ((visibility("default"))) 
#else
#define dllx extern "C"
#endif

#define trace(...) { printf("[shader_replace_unsafe:%d] ", __LINE__); printf(__VA_ARGS__); printf("\n"); fflush(stdout); }

#include "gml_ext.h"

// sick of these Types***
template<typename T> using PtrTo = T*;
template<typename T> using ArrayOf = T*;

template<typename T> T* malloc_arr(size_t pCount) {
	return (T*)malloc(sizeof(T) * pCount);
}
template<typename T> T* realloc_arr(T* arr, size_t pCount) {
	return (T*)realloc(arr, sizeof(T) * pCount);
}
template<typename T> T* memcpy_arr(T* dst, const T* src, size_t pCount) {
	return (T*)memcpy(dst, src, sizeof(T) * pCount);
}

// TODO: reference additional headers your program requires here