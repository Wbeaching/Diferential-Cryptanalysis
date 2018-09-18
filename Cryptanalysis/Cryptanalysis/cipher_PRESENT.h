#pragma once
#include "types.h"
#include "generalFuncs.h"

class PRESENT{
public:
	PRESENT();
	//void printSbox();
	//void printSboxDiffTable();
	//void printSboxLinearTable();
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
	int SDiffTable[inNum][outNum];
	int ISDiffTable[inNum][outNum];
	int SLinearTable[inNum][outNum];
	int ISLinearTable[inNum][outNum];
	//void genDiffTable();
	//void genLinearTable();
	//void genISbox();
	//����P��
	int permTable[blockBits];
	int ipermTable[blockBits];
	void P(byte *Pod,byte *Pid);
	void iP(byte *Pod,byte *Pid);
	ALIGNED_TYPE_(byte,16) PTable[sboxNum][inNum][sboxNum];
	ALIGNED_TYPE_(byte,16) IPTable[sboxNum][inNum][sboxNum];
	void genPTable();
	//����S�и���
	static const int diffProbNum=2;
	static const prType diffWminSbox=2;
	static const prType diffWmaxSbox=3;//����Լ�����
	prType diffProb[diffProbNum];
	int SDiffInputMaxProb[inNum];
	int ISDiffInputMaxProb[outNum];

	static const int linearProbNum=2;
	static const prType linearWminSbox=1;
	static const prType linearWmaxSbox=2;//����Լ�����
	prType linearProb[linearProbNum];
	int SLinearInputMaxProb[inNum];
	int ISLinearInputMaxProb[outNum];
	//��SP���
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
	//FILE *fp;
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