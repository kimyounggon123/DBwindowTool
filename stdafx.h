#ifndef _STDAFX_H
#define _STDAFX_H


#include <cstdint> // int_t

#include <iostream>
#include <vector>
#include <algorithm>

#include <string>
#include <tchar.h> // _t macro
#include <cstring>

#define BUFFERSIZE 1024
#define WINDOW_MODE 0
#define DEBUG_MODE 1


// memory free function

// 단일 메모리 해제
template <typename T>
bool SAFE_FREE(T*& ptr) {
	if (ptr) {
		delete ptr;
		ptr = nullptr;
		return true;
	}
	return false;
}

// 배열 메모리 해제
template <typename T>
bool SAFE_FREE_ARRAY(T*& ptr) {
	if (ptr) {
		delete[] ptr;
		ptr = nullptr;
		return true;
	}
	return false;
}


#endif