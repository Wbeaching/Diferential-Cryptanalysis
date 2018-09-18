#pragma once
#include "types.h"

byte xorSum(byte x,int len);

extern ALIGNED_TYPE_(si8,16) W16v[256*256][16];

#define GENDIFFTABLE(sbox,diffTable,inNum,outNum)\
{\
	memset(diffTable,0,sizeof(int)*inNum*outNum);\
	for(int i=0;i<inNum;i++){\
		for(int d=0;d<inNum;d++){\
			diffTable[d][sbox[i]^sbox[i^d]]++;\
		}\
	}\
}

#define GENLINEARTABLE(sbox,linearTable,inNum,outNum)\
{\
	memset(linearTable,0,sizeof(int)*inNum*outNum);\
	for (byte ii=0x0; ii<inNum; ii++){\
		byte oi = sbox[ii];\
		for (byte il=0x0; il<inNum; il++){\
			for (byte ol=0x0; ol<outNum; ol++){\
				if (xorSum(ii&il,inBits) == xorSum(oi&ol,outBits)){\
					linearTable[il][ol]++;\
				}\
			}\
		}\
	}\
	for(byte il=0x0; il<inNum; il++){\
		for(byte ol=0x0; ol<outNum; ol++){\
			int temp=linearTable[il][ol];\
			int mid=(inNum>>1);\
			linearTable[il][ol] = (temp>mid)?(temp-mid):(mid-temp);\
		}\
	}\
}