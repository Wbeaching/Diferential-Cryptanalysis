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
	void genStatistics();
	void genSPTable();
	//����·�������еı���
	static const int rounds=10;
	int round;
	prType Bnc[rounds+1];
	prType Bn[rounds+1];
	ALIGNED_TYPE_(u8,16) roundCharacter[rounds+1][sboxNum];//��һ�ִ��һ�ֵ������֡�
	si8 roundActiveSboxNum[rounds+1];
	si8 roundActiveSboxIndex[rounds+1][sboxNum];
	prType roundProb[rounds+1];
	int trailCount[rounds+1];
	FILE *fpTrails;
	std::string fpName;
	//����·�������еĺ���
	void fprintCurrentTrail();
	void foundOne();
	void getInfo(int r,__m128i tmp);
	void Round__(int r,int j,prType pr_round,__m128i tmp0);
	void Round_N_(int j,prType pr_round,__m128i tmp0);
	void Round_(int r);
	void Round_1_(int j,int a_pre,prType pr_round,__m128i tmp0);
	void Round_1();
};