#pragma once
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <time.h>
#include <xmmintrin.h>
#include <emmintrin.h>
#include <pmmintrin.h>
#include <tmmintrin.h>
#include <smmintrin.h>
#include <nmmintrin.h>
#include <immintrin.h>
//#include <math.h>
//using namespace std;
typedef unsigned char byte;
typedef unsigned int word;

#define ALIGNED_(x) __declspec(align(x))
#define ALIGNED_TYPE_(t,x) t ALIGNED_(x)

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef u16 prType;

typedef char si8;
typedef short si16;
typedef int si32;
typedef long long si64;

