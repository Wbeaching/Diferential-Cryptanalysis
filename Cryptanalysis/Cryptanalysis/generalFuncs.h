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

#define GENISBOX(isbox,sbox,inNum)\
{\
	for (byte x = 0; x<inNum; x++){\
		isbox[sbox[x]] = x;\
	}\
}

#define PRINTSBOX(sbox,isbox,inNum)\
{\
	printf("S盒:\n");\
	for(int i=0;i<inNum;i++){\
		printf("%02x ",sbox[i]);\
	}printf("\n");\
	printf("S盒的逆:\n");\
	for(int i=0;i<inNum;i++){\
		printf("%02x ",isbox[i]);\
	}printf("\n");\
}

#define PRINTTABLE(table,inNum,outNum){\
	printf("分布表\n");\
	for(int i=0;i<inNum;i++){\
		for(int j=0;j<outNum;j++){\
			printf("%d ",table[i][j]);\
		}printf("\n");\
	}printf("\n");\
}

#define P_BITPERM(permTable)\
{\
	memset(Pod,0,sboxNum);\
	int n;\
	for(int m=0;m<blockBits;m++){\
		n=permTable[m];\
		Pod[n/inBits]|=( ((Pid[m/inBits]>>(inBits-1-m%inBits))&0x1) << (inBits-1-n%inBits) );\
	}\
}

#define GENPTABLE(Num,sboxNum)\
{\
	ALIGNED_TYPE_(byte,Num) Pid[sboxNum],Pod[sboxNum];\
	for(int si=0;si<sboxNum;si++){\
		memset(Pid,0,sboxNum);\
		memset(Pod,0,sboxNum);\
		for(int ii=0;ii<inNum;ii++){\
			Pid[si]=ii;\
			P(Pod,Pid);\
			iP(Pod,Pid);\
			memcpy(PTable[si][ii],Pod,sboxNum);\
			memcpy(IPTable[si][ii],Pod,sboxNum);\
		}\
	}\
}

#define GENSTATISTICS(diffTable,full,diff_Number,diff_0_Number,diff_0_Offset,diff_1_Number,diff_1_Offset,diff_1_Non0Num,diff_1_Non0Val,diffProbNum)\
{\
	diff_Number=0;\
	memset(diff_1_Number,0,diffProbNum*sizeof(int));\
	memset(diff_0_Number,0,diffProbNum*outNum*sizeof(int));\
	for(int i=0;i<inNum;i++){\
		for(int o=0;o<outNum;o++){\
			if(diffTable[i][o]!=0&&diffTable[i][o]!=full){\
				diff_Number++;\
				diff_1_Number[(diffProbNum-diffTable[i][o]/2)]++;\
				diff_0_Number[i][(diffProbNum-diffTable[i][o]/2)]++;\
			}\
		}\
		diff_0_Offset[i][0][0]=0;\
		diff_0_Offset[i][0][1]=diff_0_Number[i][0];\
		for(int p=1;p<diffProbNum;p++){\
			diff_0_Offset[i][p][0]=diff_0_Offset[i][p-1][1];\
			diff_0_Offset[i][p][1]=diff_0_Offset[i][p-1][1]+diff_0_Number[i][p];\
		}\
	}\
	diff_1_Offset[0][0]=0;\
	diff_1_Offset[0][1]=diff_1_Number[0];\
	for(int p=1;p<diffProbNum;p++){\
		diff_1_Offset[p][0]=diff_1_Offset[p-1][1];\
		diff_1_Offset[p][1]=diff_1_Offset[p-1][1]+diff_1_Number[p];\
	}\
	for(int p=0;p<diffProbNum;p++){\
		diff_1_Non0Num[p]=0;\
	}\
	for(int p=0;p<diffProbNum;p++){\
		for(int i=0;i<outNum;i++){\
			if(diff_0_Number[i][p]!=0){\
				diff_1_Non0Val[p][diff_1_Non0Num[p]]=i;\
				diff_1_Non0Num[p]++;\
			}\
		}\
	}\
}

#define GENMAXPROB(SDiffInputMaxProb,SDiff_0_Number,diffProbNum,inNum)\
{\
	SDiffInputMaxProb[0]=0;\
	for(int i=1;i<inNum;i++){\
		for(int j=0;j<diffProbNum;j++){\
			if(SDiff_0_Number[i][j]!=0){\
				SDiffInputMaxProb[i]=j;\
				break;\
			}\
		}\
	}\
}

#define GENSPTABLE(SPTable,SDiffTable,PTable,full,SDiff_0_Offset,SDiff_1_Offset,diffProbNum)\
{\
	int diff_1_Count[diffProbNum];\
	int diff_0_Count[diffProbNum];\
	for(int p=0;p<diffProbNum;p++){\
		diff_1_Count[p]=SDiff_1_Offset[p][0];\
	}\
	for(int i=0;i<inNum;i++){\
		for(int p=0;p<diffProbNum;p++){\
			diff_0_Count[p]=SDiff_0_Offset[Wto[i]][p][0];\
		}\
		for(int o=0;o<outNum;o++){\
			if(SDiffTable[Wto[i]][Wto[o]]!=0&&SDiffTable[Wto[i]][Wto[o]]!=full){\
				for(int si=0;si<sboxNum;si++){\
					memcpy(SPTable[si][Wto[i]][diff_0_Count[diffProbNum-SDiffTable[Wto[i]][Wto[o]]/2]],PTable[si][Wto[o]],sboxNum*sizeof(byte));\
				}\
				diff_1_Count[diffProbNum-SDiffTable[Wto[i]][Wto[o]]/2]++;\
				diff_0_Count[diffProbNum-SDiffTable[Wto[i]][Wto[o]]/2]++;\
			}\
		}\
	}\
}