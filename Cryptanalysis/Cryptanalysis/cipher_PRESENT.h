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
	//cipher�Ĺ��
	static const int inBits=4;
	static const int outBits=4;
	static const int sboxNum=16;
	static const int blockBits=inBits*sboxNum;
	static const int inNum=1<<inBits;
	static const int outNum=1<<outBits;
	//����S��
	byte sbox[inNum];
	byte isbox[outNum];//inNum==outNumʱsbox������
	int diffTable[inNum][outNum];
	int linearTable[inNum][outNum];
	void genDiffTable();
	void genLinearTable();
	void genISbox();
	//����P��
	int PTable[blockBits];
	int iPTable[blockBits];
	void P(byte *Pod,byte *Pid);
	void iP(byte *Pod,byte *Pid);
	ALIGNED_TYPE_(byte,16) permTable[sboxNum][inNum][sboxNum];
	ALIGNED_TYPE_(byte,16) ipermTable[sboxNum][inNum][sboxNum];
	void genPermTable();
	//����S�и���
	static const int diffProbNum=2;
	static const prType diffWminSbox=2;
	static const prType diffWmaxSbox=3;//����Լ�����
	prType diffProb[diffProbNum];
	int diffInputMaxProb[inNum];
	int diffOutputMaxProb[outNum];
	//��SP���
	int diff_Number;
	int diff_0_Number[outNum][diffProbNum];
	int diff_0_Offset[outNum][diffProbNum][2];
	int diff_1_Number[diffProbNum];
	int diff_1_Offset[diffProbNum][2];
	int diff_1_Non0Num[diffProbNum];
	byte diff_1_Non0Val[diffProbNum][outNum];
	byte Wto[16];
	ALIGNED_TYPE_(byte,16) SPTable[sboxNum][inNum][inNum/2][sboxNum];
	//ALIGNED_TYPE_(byte,16) iSiPTable[sboxNum][inNum][inNum/2][sboxNum];
	//byte STable[inNum][inNum/2];
	//byte iSTable[inNum][inNum/2];
	void genStatistics();
	void genSPTable();
	//����·�������еı���
	static const int rounds=40;
	int round;
	prType Bnc[rounds];
	prType Bn[rounds];
	ALIGNED_TYPE_(u8,16) roundCharacteristic[rounds+1][sboxNum];//��һ�ִ��һ�ֵ������֡�
	ALIGNED_TYPE_(u8,16) roundCharacteristic1[sboxNum];
	si8 roundActiveSboxNum[rounds+1];
	si8 roundActiveSboxIndex[rounds+1][sboxNum];
	prType roundProb[rounds+1];
	int trailCount[rounds];
	FILE *fpTrails;
	string fpName;
	//����·�������еĺ���
	void fprintCurrentTrail();
	void foundOne();
	void getInfo(int r,__m128i tmp);
	void searchRound(int r,int j,prType pr_round,__m128i tmp0);
	void searchRoundN(int j,prType pr_round,__m128i tmp0);
	void searchRound(int r);
	void searchRound1(int j,int a_pre,prType pr_round,__m128i tmp0);
	void searchRound1();
	void traverseRound1();
	void traverseRound1(int j,__m128i tmp0);
};