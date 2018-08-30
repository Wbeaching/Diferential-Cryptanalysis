#pragma once
#include "types.h"
#include "generalFuncs.h"

class PRESENT{
public:
	PRESENT();
	void printSbox();
	void printSboxDiffTable();
	void printSboxLinearTable();
	void fprintPermTable();
	void fprintSPTable();
	void fprintStatistics();
	void searchForBestDiffTrails();
private:
	//cipher的规格
	static const int inBits=4;
	static const int outBits=4;
	static const int sboxNum=16;
	static const int blockBits=inBits*sboxNum;
	static const int inNum=1<<inBits;
	static const int outNum=1<<outBits;
	//关于S盒
	byte sbox[inNum];
	byte isbox[outNum];//inNum==outNum时sbox才有逆
	int diffTable[inNum][outNum];
	int linearTable[inNum][outNum];
	void genDiffTable();
	void genLinearTable();
	void genISbox();
	//关于P层
	int PTable[blockBits];
	int iPTable[blockBits];
	void P(byte *Pod,byte *Pid);
	void iP(byte *Pod,byte *Pid);
	ALIGNED_TYPE_(byte,16) permTable[sboxNum][inNum][sboxNum];
	void genPermTable();
	//关于S盒概率
	static const int diffProbNum=2;
	static const prType diffWminSbox=2;
	static const prType diffWmaxSbox=3;//最好自己生成
	prType diffProb[diffProbNum];
	int diffInputMaxProb[inNum];
	int diffOutputMaxProb[outNum];
	//建SP大表
	int diff_Number;
	int diff_0_Number[outNum][diffProbNum];
	int diff_0_Offset[outNum][diffProbNum][2];
	int diff_1_Number[diffProbNum];
	int diff_1_Offset[diffProbNum][2];
	int diff_1_Non0Num[diffProbNum];
	byte diff_1_Non0Val[diffProbNum][outNum];
	byte Wto[16];
	ALIGNED_TYPE_(byte,16) SPTable[sboxNum][inNum][inNum/2][sboxNum];
	void genStatistics();
	void genSPTable();
	//搜索路径过程中的变量
	static const int rounds=40;
	int round;
	prType Bnc[rounds];
	prType Bn[rounds];
	ALIGNED_TYPE_(u8,16) roundCharacteristic[rounds+1][sboxNum];//第一轮存第一轮的输出差分。
	si8 roundActiveSboxNum[rounds+1];
	si8 roundActiveSboxIndex[rounds+1][sboxNum];
	prType roundProb[rounds+1];
	int trailCount[rounds];
	FILE *fpTrails;
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
};