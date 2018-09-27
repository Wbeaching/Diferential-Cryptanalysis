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

#define PRINTSBOX(sbox,inNum)\
{\
	printf("S盒:\n");\
	for(int i=0;i<inNum;i++){\
		printf("%02x ",sbox[i]);\
	}printf("\n");\
}

#define PRINTTABLE(table,inNum,outNum){\
	printf("分布表\n");\
	for(int i=0;i<inNum;i++){\
		printf("%02x: ",i);\
		for(int j=0;j<outNum;j++){\
			printf("%d ",table[i][j]);\
		}printf("\n");\
	}printf("\n");\
}

#define P_BITPERM(permTable,uType,blockBits,PInBits,POutBits)\
{\
	memset(Pod,0,sizeof(uType)*sboxNum);\
	int n;\
	for(int m=0;m<blockBits;m++){\
		n=permTable[m];\
		/*Pod[n/POutBits]|=( ((Pid[m/PInBits]>>(PInBits-1-m%PInBits))&0x1) << (POutBits-1-n%POutBits) );*/\
		Pod[m/POutBits]|=( ((Pid[n/PInBits]>>(PInBits-1-n%PInBits))&0x1) << (POutBits-1-m%POutBits) );\
	}\
}

#define GENPTABLE(Num,sboxNum,uType,P,PTable,inNum)\
{\
	ALIGNED_TYPE_(uType,Num) Pid[sboxNum],Pod[sboxNum];\
	for(int si=0;si<sboxNum;si++){\
		memset(Pid,0,sizeof(uType)*sboxNum);\
		memset(Pod,0,sizeof(uType)*sboxNum);\
		for(int ii=0;ii<inNum;ii++){\
			Pid[si]=ii;\
			P(Pod,Pid);\
			memcpy(PTable[si][ii],Pod,sizeof(uType)*sboxNum);\
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

#define FPRINTSTATISTICS(SDiff_Number,SDiff_0_Number,SDiff_0_Offset,SDiff_1_Number,SDiff_1_Offset,SDiff_1_Non0Num,SDiff_1_Non0Val,diffProbNum,diffProb,SDiffInputMaxProb,ISDiffInputMaxProb)\
{\
	fp=fopen("statistics.txt","w");\
	fprintf(fp,"diff_0_Offset:\n");\
	for(int i=0x0;i<inNum;i++){\
		fprintf(fp,"*/0x%02x*/",i);\
		for(int p=0x0;p<diffProbNum;p++){\
			fprintf(fp,"%d:%d-(%d,%d)\t",p,SDiff_0_Number[i][p],SDiff_0_Offset[i][p][0],SDiff_0_Offset[i][p][1]);\
		}\
		fprintf(fp,"\n");\
	}\
	fprintf(fp,"diff_1_Offset:\n");\
	for(int p=0x0;p<diffProbNum;p++){\
		fprintf(fp,"%d:%d-(%d,%d)\t",p,SDiff_1_Number[p],SDiff_1_Offset[p][0],SDiff_1_Offset[p][1]);\
	}\
	fprintf(fp,"\n");\
	fprintf(fp,"diff_1_Non0:\n");\
	for(int p=0;p<diffProbNum;p++){\
		fprintf(fp,"%d:%d\n",p,SDiff_1_Non0Num[p]);\
		for(int i=0;i<SDiff_1_Non0Num[p];i++){\
			fprintf(fp,"%02x\t",SDiff_1_Non0Val[p][i]);\
		}\
		fprintf(fp,"\n");\
	}\
	fprintf(fp,"diffProb:\n");\
	for(int p=0;p<diffProbNum;p++){\
		fprintf(fp,"%d\t",diffProb[p]);\
	}\
	fprintf(fp,"\n");\
	fprintf(fp,"diffInputMaxProb:\n");\
	for(int i=0;i<inNum;i++){\
		fprintf(fp,"%d\t",SDiffInputMaxProb[i]);\
	}\
	fprintf(fp,"\n");\
	fprintf(fp,"diffOutputMaxProb:\n");\
	for(int o=0;o<outNum;o++){\
		fprintf(fp,"%d\t",ISDiffInputMaxProb[o]);\
	}\
	fprintf(fp,"\n");\
	fclose(fp);\
}

#define FPRINTSPTABLE(diffSPTable,diffProbNum,SDiff_0_Offset)\
{\
	fp=fopen("SPTable.txt","w");\
	for(int si=0;si<sboxNum;si++){\
		fprintf(fp,"以下是第%d个S盒的SP表\n",si);\
		for(int i=0x0;i<outNum;i++){\
			fprintf(fp,"*/0x%02x*/\t",i);\
			for(int p=0x0;p<diffProbNum;p++){\
				fprintf(fp,"%d:\t",p);\
				for(int k=SDiff_0_Offset[i][p][0];k<SDiff_0_Offset[i][p][1];k++){\
					fprintf(fp,"(");\
					for(int l=0;l<16;l++){\
						fprintf(fp,"%02x,",diffSPTable[si][i][k][l]);\
					}\
					fprintf(fp,")\t");\
				}\
			}\
			fprintf(fp,"\n");\
		}\
	}\
	fclose(fp);\
}

#define FPRINTPTABLE(PTable,PInNum)\
{\
	fp=fopen("PTable.txt","w");\
	for(int s=0;s<sboxNum;s++){\
		for(int id=0;id<PInNum;id++){\
			fprintf(fp,"/*0x%02x*/{",id);\
			for(int i=0;i<sboxNum;i++){\
				fprintf(fp,"0x%02x,",PTable[s][id][i]);\
			}\
			fprintf(fp,"},\n");\
		}\
		fprintf(fp,"},\n以上是第%d个S盒\n",s);\
	}\
	fclose(fp);\
}

#define FPRINTCURRENTTRAIL()\
{\
	fprintf(fpTrails,"%d:\n",trailCount[round]);\
	for(int r=0;r<round;r++){\
		for(int si=0;si<sboxNum;si++){\
			fprintf(fpTrails,"%01x,",roundCharacteristic[r][si]);\
		}fprintf(fpTrails,"\t");\
		fprintf(fpTrails,"%d",roundProb[r+1]-roundProb[r]);\
		fprintf(fpTrails,"\n");\
	}\
	for(int si=0;si<sboxNum;si++){\
		fprintf(fpTrails,"%01x,",roundCharacteristic[round][si]);\
	}fprintf(fpTrails,"\t");\
	fprintf(fpTrails,"total:%d",roundProb[round]);\
	fprintf(fpTrails,"\n");\
}

#define FPRINTCLUSTER()\
{\
	for(int r=0;r<round;r++){\
		for(int si=0;si<sboxNum;si++){\
			fprintf(fpTrails,"%01x ",roundCharacteristic[r][si]);\
		}\
		fprintf(fpTrails,"\n");\
	}\
	for(int si=0;si<sboxNum;si++){\
		fprintf(fpTrails,"%01x ",roundCharacteristic[round][si]);\
	}\
	fprintf(fpTrails,"\n");\
}