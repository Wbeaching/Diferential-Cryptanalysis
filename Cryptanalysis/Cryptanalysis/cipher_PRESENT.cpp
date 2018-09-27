#include "cipher_PRESENT.h"

PRESENT::PRESENT(){
	byte sboxArr[inNum]={0xC, 0x5, 0x6, 0xB , 0x9 , 0x0 , 0xA , 0xD , 0x3 , 0xE , 0xF , 0x8 , 0x4 , 0x7 , 0x1 , 0x2};
	int ipermArr[blockBits]={
	0,16,32,48,1,17,33,49,2,18,34,50,3,19,35,51,
	4,20,36,52,5,21,37,53,6,22,38,54,7,23,39,55,
	8,24,40,56,9,25,41,57,10,26,42,58,11,27,43,59,
	12,28,44,60,13,29,45,61,14,30,46,62,15,31,47,63};
	int permArr[blockBits]={
	0,4,8,12,16,20,24,28,32,36,40,44,48,52,56,60,
	1,5,9,13,17,21,25,29,33,37,41,45,49,53,57,61,
	2,6,10,14,18,22,26,30,34,38,42,46,50,54,58,62,
	3,7,11,15,19,23,27,31,35,39,43,47,51,55,59,63};
	byte W[16] = {0x0, 0x1, 0x2, 0x4, 0x8, 0x3, 0x5, 0x6, 0x9, 0xa, 0xc, 0x7, 0xb, 0xd, 0xe, 0xf};

	memcpy(sbox,sboxArr,inNum);
	memcpy(permTable,permArr,sizeof(int)*blockBits);
	memcpy(ipermTable,ipermArr,sizeof(int)*blockBits);
	memcpy(Wto,W,sizeof(byte)*16);
	memset(trailCount,0,sizeof(int)*(rounds+1));

	GENDIFFTABLE(sbox,SDiffTable,inNum,outNum);
	GENLINEARTABLE(sbox,SLinearTable,inNum,outNum);
	GENISBOX(isbox,sbox,inNum);
	GENDIFFTABLE(isbox,ISDiffTable,outNum,inNum);
	GENLINEARTABLE(isbox,ISLinearTable,outNum,inNum);
	GENPTABLE(16,sboxNum,byte,P,PTable,inNum);
	GENPTABLE(16,sboxNum,byte,iP,IPTable,inNum);
	
	PRINTSBOX(sbox,inNum);
	PRINTSBOX(isbox,outNum);
	PRINTTABLE(SDiffTable,inNum,outNum);
	PRINTTABLE(SLinearTable,inNum,outNum);

	GENSTATISTICS(SDiffTable,inNum,SDiff_Number,SDiff_0_Number,SDiff_0_Offset,SDiff_1_Number,SDiff_1_Offset,SDiff_1_Non0Num,SDiff_1_Non0Val,diffProbNum);
	GENSTATISTICS(ISDiffTable,inNum,ISDiff_Number,ISDiff_0_Number,ISDiff_0_Offset,ISDiff_1_Number,ISDiff_1_Offset,ISDiff_1_Non0Num,ISDiff_1_Non0Val,diffProbNum);
	GENSTATISTICS(SLinearTable,inNum/2,SLinear_Number,SLinear_0_Number,SLinear_0_Offset,SLinear_1_Number,SLinear_1_Offset,SLinear_1_Non0Num,SLinear_1_Non0Val,linearProbNum);
	GENSTATISTICS(ISLinearTable,inNum/2,ISLinear_Number,ISLinear_0_Number,ISLinear_0_Offset,ISLinear_1_Number,ISLinear_1_Offset,ISLinear_1_Non0Num,ISLinear_1_Non0Val,linearProbNum);
	GENMAXPROB(SDiffInputMaxProb,SDiff_0_Number,diffProbNum,inNum);
	GENMAXPROB(ISDiffInputMaxProb,ISDiff_0_Number,diffProbNum,outNum);
	GENMAXPROB(SLinearInputMaxProb,SLinear_0_Number,linearProbNum,inNum);
	GENMAXPROB(ISLinearInputMaxProb,ISLinear_0_Number,linearProbNum,outNum);

	FPRINTPTABLE(PTable,inNum);
	FPRINTSTATISTICS(SDiff_Number,SDiff_0_Number,SDiff_0_Offset,SDiff_1_Number,SDiff_1_Offset,SDiff_1_Non0Num,SDiff_1_Non0Val,diffProbNum,diffProb,SDiffInputMaxProb,ISDiffInputMaxProb);

	diffProb[0]=2;
	diffProb[1]=3;
	linearProb[0]=1;
	linearProb[1]=2;

	GENSPTABLE(diffSPTable,SDiffTable,PTable,inNum,SDiff_0_Offset,SDiff_1_Offset,diffProbNum);
	GENSPTABLE(diffISIPTable,ISDiffTable,IPTable,inNum,ISDiff_0_Offset,ISDiff_1_Offset,diffProbNum);
	GENSPTABLE(linearSPTable,SLinearTable,PTable,inNum/2,SLinear_0_Offset,SLinear_1_Offset,linearProbNum);
	GENSPTABLE(linearISIPTable,ISLinearTable,IPTable,inNum/2,SLinear_0_Offset,SLinear_1_Offset,linearProbNum);

	FPRINTSPTABLE(diffSPTable,diffProbNum,SDiff_0_Offset);
	/*for(int i=0;i<round+1;i++){
		dp[i]=(__m128i *)(roundCharacteristic[i]);
	}
	__m128i tmp;
	tmp=_mm_setzero_si128();
	_mm_store_si128(dp[1],tmp);*/
}

void PRESENT::P(byte *Pod,byte *Pid){
	P_BITPERM(permTable,byte,blockBits,inBits,inBits);
}

void PRESENT::iP(byte *Pod,byte *Pid){
	P_BITPERM(ipermTable,byte,blockBits,inBits,inBits);
}

void PRESENT::fprintStatistics(){
	FPRINTSTATISTICS(SDiff_Number,SDiff_0_Number,SDiff_0_Offset,SDiff_1_Number,SDiff_1_Offset,SDiff_1_Non0Num,SDiff_1_Non0Val,diffProbNum,diffProb,SDiffInputMaxProb,ISDiffInputMaxProb);
}

void PRESENT::fprintSPTable(){
	FPRINTSPTABLE(diffSPTable,diffProbNum,SDiff_0_Offset)
}

void PRESENT::fprintPermTable(){
	FPRINTPTABLE(PTable,inNum);
}

void PRESENT::fprintCurrentTrail(){
	FPRINTCURRENTTRAIL();
}

void PRESENT::foundOne(){
	if(Bnc[round]==roundProb[round]){
		trailCount[round]++;
	}else{
		Bnc[round]=roundProb[round];
		trailCount[round]=1;
	}
	fprintCurrentTrail();
}

void PRESENT::getInfo(int r,__m128i tmp){
	__m128i a;
	__m128i c;
	int lm;
	int la;

	a=_mm_setzero_si128();
	a=_mm_cmpgt_epi8(tmp,a);//tmp非零的8比特置为0xff
	lm=_mm_movemask_epi8(a);
	la=_mm_popcnt_u32(lm);
	roundActiveSboxNum[r]=la;

	c=_mm_load_si128((__m128i *)(&(W16v[lm][0])));
	_mm_storeu_si128((__m128i *)(roundActiveSboxIndex[r]),c);
}

void PRESENT::searchRound(int r,int j,prType pr_round,__m128i tmp0){
	si8 an=roundActiveSboxNum[r-1];
	si8 ai=roundActiveSboxIndex[r-1][j];
	si8 an_remain=an-j-1;
	prType prob;
	si8 s;
	si8 m;
	u16 idv=roundCharacteristic[r-1][ai];
	__m128i tmp1;
	__m128i *odp;
	odp=(__m128i *)(roundCharacteristic[r]);
#if (DIFF==1)
	for(si16 pr=0;pr<diffProbNum;pr++){//遍历概率
		prob=diffProb[pr]+pr_round;
		if((prob+roundProb[r-1]+Bn[round-r]+diffWminSbox*an_remain)<=(Bnc[round])){
			s=SDiff_0_Offset[idv][pr][0];
			m=SDiff_0_Number[idv][pr];
			for(si16 k=s;k<s+m;k++){//遍历输出差分
				tmp1=_mm_xor_si128(tmp0,*(__m128i *)(diffSPTable[ai][idv][k]));
#else
	for(si16 pr=0;pr<linearProbNum;pr++){//遍历概率
		prob=linearProb[pr]+pr_round;
		if((prob+roundProb[r-1]+Bn[round-r]+linearWminSbox*an_remain)<=(Bnc[round])){
			s=SLinear_0_Offset[idv][pr][0];
			m=SLinear_0_Number[idv][pr];
			for(si16 k=s;k<s+m;k++){//遍历输出差分
				tmp1=_mm_xor_si128(tmp0,*(__m128i *)(linearSPTable[ai][idv][k]));
#endif
				if(an_remain==0){
					roundProb[r]=roundProb[r-1]+prob;
					_mm_store_si128(odp,tmp1);
					searchRound(r+1);
				}else{
					searchRound(r,j+1,prob,tmp1);
				}
			}
		}else break;
	}
}

void PRESENT::searchRoundN(int j,prType pr_round,__m128i tmp0){
	si8 an=roundActiveSboxNum[round-1];
	si8 ai=roundActiveSboxIndex[round-1][j];
	si8 an_remain=an-j-1;
	prType prob;
	u16 idv=roundCharacteristic[round-1][ai];
	
	si8 s;
	si8 m;
	__m128i tmp1;
	__m128i *odp;
	odp=(__m128i *)(roundCharacteristic[round]);
#if (DIFF==1)
	si8 pr=SDiffInputMaxProb[idv];
	prob=diffProb[pr]+pr_round;
	if((prob+roundProb[round-1]+diffWminSbox*an_remain)<=(Bnc[round])){
		s=SDiff_0_Offset[idv][pr][0];
		m=SDiff_0_Number[idv][pr];
		for(si16 k=s;k<s+m;k++){//遍历输出差分
			tmp1=_mm_xor_si128(tmp0,*(__m128i *)(diffSPTable[ai][idv][k]));
#else
	si8 pr=SLinearInputMaxProb[idv];
	prob=linearProb[pr]+pr_round;
	if((prob+roundProb[round-1]+linearWminSbox*an_remain)<=(Bnc[round])){
		s=SLinear_0_Offset[idv][pr][0];
		m=SLinear_0_Number[idv][pr];
		for(si16 k=s;k<s+m;k++){//遍历输出差分
			tmp1=_mm_xor_si128(tmp0,*(__m128i *)(linearSPTable[ai][idv][k]));
#endif
			if(an_remain==0){
				roundProb[round]=roundProb[round-1]+prob;
				_mm_store_si128(odp,tmp1);
				//foundOne();
				traverseRound1();
			}else{
				searchRoundN(j+1,prob,tmp1);
			}
		}
	}
}

void PRESENT::searchRound(int r){
	__m128i tmp1;
	tmp1=_mm_setzero_si128();
	__m128i *idp;
	idp=(__m128i *)(roundCharacteristic[r-1]);
	getInfo(r-1,*idp);
#if (DIFF==1)
	if((roundProb[r-1]+Bn[round-r]+diffWminSbox*roundActiveSboxNum[r-1])<=(Bnc[round])){}
#else
	if((roundProb[r-1]+Bn[round-r]+linearWminSbox*roundActiveSboxNum[r-1])<=(Bnc[round])){}
#endif
	else return;
	if(r==round){
		searchRoundN(0,0,tmp1);//1是起始活跃S盒，0是起始概率
	}else{
		searchRound(r,0,0,tmp1);//r是轮数，0是起始活跃S盒，0是起始概率，tmp1是起始输出差分
	}
}

void PRESENT::searchRound1(int j,int a_pre,prType pr_round,__m128i tmp0){
	prType prob;
	int a_cur;
	si16 dy;
	__m128i tmp1;
	__m128i *odp;
	odp=(__m128i *)(roundCharacteristic[1]);

	for(a_cur=a_pre+1;a_cur<sboxNum;a_cur++){
		memset(roundCharacteristic1+a_pre+1,0,sizeof(u8)*(a_cur-a_pre-1));
		for(si16 o=1;o<outNum;o++){
#if (DIFF==1)
			prob=diffProb[ISDiffInputMaxProb[o]]+pr_round;
#else
			prob=linearProb[ISLinearInputMaxProb[o]]+pr_round;
#endif
			if((prob+Bn[round-1])<=(Bnc[round])){
				roundCharacteristic1[a_cur]=o;
				roundProb[1]=prob;
				memset(roundCharacteristic1+a_cur+1,0,sizeof(u8)*(sboxNum-1-a_cur));
				tmp1=_mm_xor_si128(tmp0,*(__m128i *)(PTable[a_cur][o]));
				_mm_store_si128(odp,tmp1);
				searchRound(2);
				searchRound1(j+1,a_cur,prob,tmp1);
			}else break;
		}
	}
}

void PRESENT::searchRound1(){
	fpName=to_string((_ULonglong)(round)) +"-round_trails.txt";
	errno_t err;
	err = fopen_s(&fpTrails, fpName.c_str(), "w" );

	__m128i tmp;
	tmp=_mm_setzero_si128();
	roundProb[0]=0;
	searchRound1(1,-1,0,tmp);

	fclose(fpTrails);
}

void PRESENT::searchForBestDiffTrails(){
	Bn[0]=0;
#if (DIFF==1)
	Bn[1]=diffWminSbox;
	Bn[2]=2*diffWminSbox;
#else
	Bn[1]=linearWminSbox;
	Bn[2]=2*linearWminSbox;
#endif
	for(round=3;round<=rounds;round++){
		prType BnLowerBound=Bn[1]+Bn[round-1];
		for(int i=1;i<round/2;i++){
			prType tmp=Bn[i+1]+Bn[round-1-i];
			if(tmp>BnLowerBound){
				BnLowerBound=tmp;
			}
		}
		for(prType step=BnLowerBound;;step++){
			Bnc[round]=step;
			clock_t start,end;
			start=clock();
			searchRound1();
			end=clock();
			
			if(trailCount[round]!=0){
				printf("round:%d Bnc:%d time:%f count:%d\n",round,Bnc[round],(double)(end-start)/CLK_TCK,trailCount[round]);
				Bn[round]=Bnc[round];
				break;
			}
		}
	}
}

void PRESENT::traverseRound1(){
	__m128i tmp1;
	tmp1=_mm_setzero_si128();

	__m128i *odp;
	odp=(__m128i *)(roundCharacteristic1);
	getInfo(0,*odp);

	__m128i *idp;
	idp=(__m128i *)(roundCharacteristic[0]);
	_mm_store_si128(idp,tmp1);

	traverseRound1(0);
}

void PRESENT::traverseRound1(int j){
	si8 an=roundActiveSboxNum[0];
	si8 ai=roundActiveSboxIndex[0][j];
	si8 an_remain=an-j-1;
	prType prob;
	u16 idv=roundCharacteristic1[ai];

#if (DIFF==1)
	si8 pr=ISDiffInputMaxProb[idv];
	for(int i=1;i<inNum;i++){
		if(SDiffTable[i][idv]==(1<<(4-diffProb[pr]))){
#else
	si8 pr=ISLinearInputMaxProb[idv];
	for(int i=1;i<inNum;i++){
		if(SLinearTable[i][idv]==(1<<(3-linearProb[pr]))){
#endif
			roundCharacteristic[0][ai]=i;
			if(an_remain==0){
				foundOne();
			}else{
				traverseRound1(j+1);
			}
		}
	}
}

void PRESENT::searchForCluster(byte plaintext[sboxNum],byte ciphertext[sboxNum],prType b,int r){
	memcpy(roundCharacteristic[0],plaintext,sizeof(byte)*sboxNum);
	memcpy(roundCharacteristic1,ciphertext,sizeof(byte)*sboxNum);
	round=r;
	Bnc[r]=b;
	trailCount[r]=0;

	fpName=to_string((_ULonglong)(round)) +"-round_clusterTrails.txt";
	errno_t err;
	err = fopen_s(&fpTrails, fpName.c_str(), "w" );

	roundProb[0]=0;

	clock_t start,end;
	start=clock();
	clusterSearchRound(1);
	end=clock();
	
	fclose(fpTrails);
	printf("round:%d Bnc:%d time:%f count:%d\n",round,Bnc[r],(double)(end-start)/CLK_TCK,trailCount[r]);
}

void PRESENT::clusterSearchRound(int r){
	__m128i tmp1;
	tmp1=_mm_setzero_si128();
	__m128i *idp;
	idp=(__m128i *)(roundCharacteristic[r-1]);
	getInfo(r-1,*idp);
#if (DIFF==1)
	if((roundProb[r-1]+Bn[round-r]+diffWminSbox*roundActiveSboxNum[r-1])<=(Bnc[round])){}
#else
	if((roundProb[r-1]+Bn[round-r]+linearWminSbox*roundActiveSboxNum[r-1])<=(Bnc[round])){}
#endif
	else return;
	if(r==round){
		clusterSearchRoundN(0,0,tmp1);//1是起始活跃S盒，0是起始概率
	}else{
		clusterSearchRound(r,0,0,tmp1);//r是轮数，0是起始活跃S盒，0是起始概率，tmp1是起始输出差分
	}
}

void PRESENT::clusterSearchRoundN(int j,prType pr_round,__m128i tmp0){
	si8 an=roundActiveSboxNum[round-1];
	si8 ai=roundActiveSboxIndex[round-1][j];
	si8 an_remain=an-j-1;
	prType prob;
	u16 idv=roundCharacteristic[round-1][ai];
	
	si8 s;
	si8 m;
	__m128i tmp1;
	__m128i *odp;
	odp=(__m128i *)(roundCharacteristic[round]);

#if (DIFF==1)
	for(si16 pr=0;pr<diffProbNum;pr++){//遍历概率
		prob=diffProb[pr]+pr_round;
		if((prob+roundProb[round-1]+diffWminSbox*an_remain)<=(Bnc[round])){
			s=SDiff_0_Offset[idv][pr][0];
			m=SDiff_0_Number[idv][pr];
			for(si16 k=s;k<s+m;k++){//遍历输出差分
				tmp1=_mm_xor_si128(tmp0,*(__m128i *)(diffSPTable[ai][idv][k]));
#else
	for(si16 pr=0;pr<linearProbNum;pr++){//遍历概率
		prob=linearProb[pr]+pr_round;
		if((prob+roundProb[round-1]+linearWminSbox*an_remain)<=(Bnc[round])){
			s=SLinear_0_Offset[idv][pr][0];
			m=SLinear_0_Number[idv][pr];
			for(si16 k=s;k<s+m;k++){//遍历输出差分
				tmp1=_mm_xor_si128(tmp0,*(__m128i *)(linearSPTable[ai][idv][k]));
#endif
	
				if(an_remain==0){
					roundProb[round]=roundProb[round-1]+prob;
					_mm_store_si128(odp,tmp1);
					clusterFoundOne();
				}else{
					clusterSearchRoundN(j+1,prob,tmp1);
				}
			}
		}else break;
	}
}

void PRESENT::clusterSearchRound(int r,int j,prType pr_round,__m128i tmp0){
	si8 an=roundActiveSboxNum[r-1];
	si8 ai=roundActiveSboxIndex[r-1][j];
	si8 an_remain=an-j-1;
	prType prob;
	si8 s;
	si8 m;
	u16 idv=roundCharacteristic[r-1][ai];
	__m128i tmp1;
	__m128i *odp;
	odp=(__m128i *)(roundCharacteristic[r]);

#if (DIFF==1)
	for(si16 pr=0;pr<diffProbNum;pr++){//遍历概率
		prob=diffProb[pr]+pr_round;
		if((prob+roundProb[r-1]+Bn[round-r]+diffWminSbox*an_remain)<=(Bnc[round])){
			s=SDiff_0_Offset[idv][pr][0];
			m=SDiff_0_Number[idv][pr];
			for(si16 k=s;k<s+m;k++){//遍历输出差分
				tmp1=_mm_xor_si128(tmp0,*(__m128i *)(diffSPTable[ai][idv][k]));
#else
	for(si16 pr=0;pr<linearProbNum;pr++){//遍历概率
		prob=linearProb[pr]+pr_round;
		if((prob+roundProb[r-1]+Bn[round-r]+linearWminSbox*an_remain)<=(Bnc[round])){
			s=SLinear_0_Offset[idv][pr][0];
			m=SLinear_0_Number[idv][pr];
			for(si16 k=s;k<s+m;k++){//遍历输出差分
				tmp1=_mm_xor_si128(tmp0,*(__m128i *)(linearSPTable[ai][idv][k]));
#endif
				if(an_remain==0){
					roundProb[r]=roundProb[r-1]+prob;
					_mm_store_si128(odp,tmp1);
					clusterSearchRound(r+1);
				}else{
					clusterSearchRound(r,j+1,prob,tmp1);
				}
			}
		}else break;
	}
}

void PRESENT::clusterFoundOne(){
	__m128i tmp;
	__m128i tmp0;
	tmp0=_mm_setzero_si128();
	__m128i *odp;
	odp=(__m128i *)(roundCharacteristic[round]);
	__m128i *idp;
	idp=(__m128i *)(roundCharacteristic1);
	tmp=_mm_xor_si128(*idp,*odp);
	
	tmp0=_mm_cmpgt_epi8(tmp,tmp0);//tmp非零的8比特置为0xff
	int l=_mm_movemask_epi8(tmp0);
	if(l==0){
		trailCount[round]++;
		FPRINTCLUSTER();
	}
}

void PRESENT::generateRoundKeys(byte key[20],ALIGNED_TYPE_(byte,16) roundKey[32][16]){
	byte state[20];
	byte tmp[20];
	memcpy(state,key,sizeof(state));
	memcpy(roundKey[0],state,sizeof(roundKey[0]));
	for(int i=1;i<=31;i++){
		for(int k=0;k<20;k++){
			tmp[k]=((state[(15+k)%20]<<1)&0xe)^((state[(16+k)%20]>>3)&0x1);
		}
		tmp[0]=sbox[tmp[0]];
		tmp[15]^=((byte)i)>>1;
		tmp[16]^=(((byte)i)<<3)&0x8;
		memcpy(state,tmp,sizeof(state));
		memcpy(roundKey[i],state,sizeof(roundKey[i]));
	}
}