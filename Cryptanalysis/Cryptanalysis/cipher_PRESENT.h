#pragma once
#include "types.h"
#include "generalFuncs.h"

#define DIFF 0

class PRESENT{
private:
	typedef int prType;
	static const int inBits=4;
	static const int outBits=4;
	static const int sboxNum=16;
	static const int blockBits=inBits*sboxNum;
	static const int inNum=1<<inBits;
	static const int outNum=1<<outBits;
	static const int rounds=6;

	static const int diffProbNum=2;
	static const prType diffWminSbox=2;
	static const prType diffWmaxSbox=3;
	static const int linearProbNum=2;
	static const prType linearWminSbox=1;
	static const prType linearWmaxSbox=2;
public:
	PRESENT();
	void fprintPermTable();
	void fprintSPTable();
	void fprintStatistics();
	void searchForBestDiffTrails();
	void searchForCluster(byte plaintext[sboxNum],byte ciphertext[sboxNum],prType b,int r);
	prType Bn[rounds+1];
	int trailCount[rounds+1];
	void generateRoundKeys(byte key[20],ALIGNED_TYPE_(byte,16) roundKey[32][16]);
private:
	//关于S盒
	byte sbox[inNum];
	byte isbox[outNum];//inNum==outNum时sbox才有逆
	int SDiffTable[inNum][outNum];
	int ISDiffTable[inNum][outNum];
	int SLinearTable[inNum][outNum];
	int ISLinearTable[inNum][outNum];
	//关于P层
	int permTable[blockBits];
	int ipermTable[blockBits];
	void P(byte *Pod,byte *Pid);
	void iP(byte *Pod,byte *Pid);
	ALIGNED_TYPE_(byte,16) PTable[sboxNum][inNum][sboxNum];
	ALIGNED_TYPE_(byte,16) IPTable[sboxNum][inNum][sboxNum];
	void genPTable();
	//关于S盒概率
	prType diffProb[diffProbNum];
	int SDiffInputMaxProb[inNum];
	int ISDiffInputMaxProb[outNum];
	prType linearProb[linearProbNum];
	int SLinearInputMaxProb[inNum];
	int ISLinearInputMaxProb[outNum];
	//建SP大表
	int SDiff_Number;
	int SDiff_0_Number[outNum][diffProbNum];
	int SDiff_0_Offset[outNum][diffProbNum][2];
	int SDiff_1_Number[diffProbNum];
	int SDiff_1_Offset[diffProbNum][2];
	int SDiff_1_Non0Num[diffProbNum];
	byte SDiff_1_Non0Val[diffProbNum][outNum];
	int ISDiff_Number;
	int ISDiff_0_Number[outNum][diffProbNum];
	int ISDiff_0_Offset[outNum][diffProbNum][2];
	int ISDiff_1_Number[diffProbNum];
	int ISDiff_1_Offset[diffProbNum][2];
	int ISDiff_1_Non0Num[diffProbNum];
	byte ISDiff_1_Non0Val[diffProbNum][outNum];
	int SLinear_Number;
	int SLinear_0_Number[outNum][linearProbNum];
	int SLinear_0_Offset[outNum][linearProbNum][2];
	int SLinear_1_Number[linearProbNum];
	int SLinear_1_Offset[linearProbNum][2];
	int SLinear_1_Non0Num[linearProbNum];
	byte SLinear_1_Non0Val[linearProbNum][outNum];
	int ISLinear_Number;
	int ISLinear_0_Number[outNum][linearProbNum];
	int ISLinear_0_Offset[outNum][linearProbNum][2];
	int ISLinear_1_Number[linearProbNum];
	int ISLinear_1_Offset[linearProbNum][2];
	int ISLinear_1_Non0Num[linearProbNum];
	byte ISLinear_1_Non0Val[linearProbNum][outNum];
	byte Wto[16];
	ALIGNED_TYPE_(byte,16) diffSPTable[sboxNum][inNum][outNum/2][sboxNum];
	ALIGNED_TYPE_(byte,16) diffISIPTable[sboxNum][outNum][inNum/2][sboxNum];
	ALIGNED_TYPE_(byte,16) linearSPTable[sboxNum][inNum][outNum/2][sboxNum];
	ALIGNED_TYPE_(byte,16) linearISIPTable[sboxNum][outNum][inNum/2][sboxNum];
	//void genStatistics();
	void genSPTable();
	//搜索路径过程中的变量

	int round;
	prType Bnc[rounds+1];
	
	ALIGNED_TYPE_(u8,16) roundCharacteristic[rounds+1][sboxNum];//第一轮存第一轮的输出差分。
	//__m128i *dp[rounds+1];
	//vector<__m128i *> dp;
	ALIGNED_TYPE_(u8,16) roundCharacteristic1[sboxNum];
	si8 roundActiveSboxNum[rounds+1];
	si8 roundActiveSboxIndex[rounds+1][sboxNum];
	prType roundProb[rounds+1];
	
	FILE *fpTrails;
	FILE *fp;
	string fpName;
	//搜索路径过程中的函数
	void fprintCurrentTrail();

	void foundOne();
	void getInfo(int r,__m128i tmp);
	void searchRound(int r,int j,prType pr_round,__m128i tmp0);
	void searchRoundN(int j,prType pr_round,__m128i tmp0);
	void searchRound(int r);
	void searchRound1(int j,int a_pre,prType pr_round,__m128i tmp0);
	void searchRound1();
	void traverseRound1();
	void traverseRound1(int j);

	void clusterFoundOne();
	void clusterSearchRound(int r,int j,prType pr_round,__m128i tmp0);
	void clusterSearchRoundN(int j,prType pr_round,__m128i tmp0);
	void clusterSearchRound(int r);
	
};
