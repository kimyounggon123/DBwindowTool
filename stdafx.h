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
#define WINDOW_MODE 1
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


/// <summary>
/// 바이트 단위의 endian changer
/// 해당 코드는 바이트 단위로 endian을 change해 Big <-> little 형태로 만들도록 지원하는 커스텀 함수임
/// 
/// 
/// <원리>
/// 기존 값에 1바이트 단위로 마스크를 씌워 특정 위치의 바이트만 추출함. 
/// 4bytes 인 integer는 4번 실행하며, 이렇게 추출한 byte의 위치를 바꿈.
/// 이후 이동시킨 바이트 값 4개를 전부 or operator로 합침
/// </summary>
/// 


#endif