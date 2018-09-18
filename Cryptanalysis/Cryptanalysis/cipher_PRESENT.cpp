#include "cipher_PRESENT.h"

PRESENT::PRESENT(){
	byte sboxArr[inNum]={0xC, 0x5, 0x6, 0xB , 0x9 , 0x0 , 0xA , 0xD , 0x3 , 0xE , 0xF , 0x8 , 0x4 , 0x7 , 0x1 , 0x2};
	int permArr[blockBits]={
	0,16,32,48,1,17,33,49,2,18,34,50,3,19,35,51,
	4,20,36,52,5,21,37,53,6,22,38,54,7,23,39,55,
	8,24,40,56,9,25,41,57,10,26,42,58,11,27,43,59,
	12,28,44,60,13,29,45,61,14,30,46,62,15,31,47,63};
	int ipermArr[blockBits]={
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
	GENPTABLE(16,sboxNum);

	PRINTSBOX(sbox,isbox,inNum);
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

	diffProb[0]=2;
	diffProb[1]=3;
	linearProb[0]=1;
	linearProb[1]=2;

	GENSPTABLE(diffSPTable,SDiffTable,PTable,inNum,SDiff_0_Offset,SDiff_1_Offset,diffProbNum);
	GENSPTABLE(diffISIPTable,ISDiffTable,IPTable,inNum,ISDiff_0_Offset,ISDiff_1_Offset,diffProbNum);
	GENSPTABLE(linearSPTable,SLinearTable,PTable,inNum/2,SLinear_0_Offset,SLinear_1_Offset,linearProbNum);
	GENSPTABLE(linearISIPTable,ISLinearTable,IPTable,inNum/2,SLinear_0_Offset,SLinear_1_Offset,linearProbNum);
}

void PRESENT::P(byte *Pod,byte *Pid){
	P_BITPERM(permTable);
}

void PRESENT::iP(byte *Pod,byte *Pid){
	P_BITPERM(ipermTable);
}

void PRESENT::fprintStatistics(){
	FILE* fp;
	fp=fopen("statistics.txt","w");
	fprintf(fp,"diff_0_Offset:\n");
	for(int i=0x0;i<inNum;i++){
		fprintf(fp,"*/0x%02x*/",i);
		for(int p=0x0;p<diffProbNum;p++){
			fprintf(fp,"%d:%d-(%d,%d)\t",p,SDiff_0_Number[i][p],SDiff_0_Offset[i][p][0],SDiff_0_Offset[i][p][1]);
		}
		fprintf(fp,"\n");
	}
	fprintf(fp,"diff_1_Offset:\n");
	for(int p=0x0;p<diffProbNum;p++){
		fprintf(fp,"%d:%d-(%d,%d)\t",p,SDiff_1_Number[p],SDiff_1_Offset[p][0],SDiff_1_Offset[p][1]);
	}
	fprintf(fp,"\n");

	fprintf(fp,"diff_1_Non0:\n");
	for(int p=0;p<diffProbNum;p++){
		fprintf(fp,"%d:%d\n",p,SDiff_1_Non0Num[p]);
		for(int i=0;i<SDiff_1_Non0Num[p];i++){
			fprintf(fp,"%02x\t",SDiff_1_Non0Val[p][i]);
		}
		fprintf(fp,"\n");
	}

	fprintf(fp,"diffProb:\n");
	for(int p=0;p<diffProbNum;p++){
		fprintf(fp,"%d\t",diffProb[p]);
	}
	fprintf(fp,"\n");
	fprintf(fp,"diffInputMaxProb:\n");
	for(int i=0;i<inNum;i++){
		fprintf(fp,"%d\t",SDiffInputMaxProb[i]);
	}
	fprintf(fp,"\n");
	fprintf(fp,"diffOutputMaxProb:\n");
	for(int o=0;o<outNum;o++){
		fprintf(fp,"%d\t",ISDiffInputMaxProb[o]);
	}
	fprintf(fp,"\n");
	fclose(fp);
}

void PRESENT::fprintSPTable(){
	FILE* fp;
	fp=fopen("SPTable.txt","w");
	for(int si=0;si<sboxNum;si++){
		fprintf(fp,"以下是第%d个S盒的SP表\n",si);
		for(int i=0x0;i<outNum;i++){
			fprintf(fp,"*/0x%02x*/\t",i);
			for(int p=0x0;p<diffProbNum;p++){
				fprintf(fp,"%d:\t",p);
				for(int k=SDiff_0_Offset[i][p][0];k<SDiff_0_Offset[i][p][1];k++){
					fprintf(fp,"(");
					for(int l=0;l<16;l++){
						fprintf(fp,"%02x,",diffSPTable[si][i][k][l]);
					}
					fprintf(fp,")\t");
				}
			}
			fprintf(fp,"\n");
		}
	}
	fclose(fp);
}

void PRESENT::fprintPermTable(){
	FILE* fp;
	fp=fopen("perm.txt","w");
	for(int s=0;s<sboxNum;s++){
		for(int id=0;id<inNum;id++){
			fprintf(fp,"/*0x%02x*/{",id);
			for(int i=0;i<sboxNum;i++){
				fprintf(fp,"0x%02x,",PTable[s][id][i]);
			}
			fprintf(fp,"},\n");
		}
		fprintf(fp,"},\n以上是第%d个S盒\n",s);
	}
	fclose(fp);
}

void PRESENT::fprintCurrentTrail(){
	fprintf(fpTrails,"%d:\n",trailCount[round-1]+1);
	for(int r=0;r<round;r++){
		for(int si=0;si<sboxNum;si++){
			fprintf(fpTrails,"%01x ",roundCharacteristic[r][si]);
		}fprintf(fpTrails,"\t");
		fprintf(fpTrails,"\t%d",roundProb[r]);
		fprintf(fpTrails,"\n");
	}
}

void PRESENT::foundOne(){
	Bnc[round-1]=roundProb[round-1];
	fprintCurrentTrail();
	trailCount[round-1]++;
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

	for(si16 pr=0;pr<diffProbNum;pr++){//遍历概率
		prob=diffProb[pr]+pr_round;
		if((prob+roundProb[r-2]+Bn[round-r-1]+diffWminSbox*an_remain)<=(Bnc[round-1])){
			s=SDiff_0_Offset[idv][pr][0];
			m=SDiff_0_Number[idv][pr];
			for(si16 k=s;k<s+m;k++){//遍历输出差分
				tmp1=_mm_xor_si128(tmp0,*(__m128i *)(diffSPTable[ai][idv][k]));
				if(an_remain==0){
					roundProb[r-1]=roundProb[r-2]+prob;
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

	si8 pr=SDiffInputMaxProb[idv];
	prob=diffProb[pr]+pr_round;
	if((prob+roundProb[round-2]+diffWminSbox*an_remain)<=(Bnc[round-1])){
		s=SDiff_0_Offset[idv][pr][0];
		m=SDiff_0_Number[idv][pr];
		for(si16 k=s;k<s+m;k++){//遍历输出差分
			tmp1=_mm_xor_si128(tmp0,*(__m128i *)(diffSPTable[ai][idv][k]));
			if(an_remain==0){
				roundProb[round-1]=roundProb[round-2]+prob;
				_mm_store_si128(odp,tmp1);
				foundOne();
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
	/*FILE* fq;
	fq=fopen("test.txt","a+");
	fprintf(fq,"%d %d %d\n",round,r,Bn[round-r-1]);
	fclose(fq);*/
	//if((roundProb[r-2]+Bn[round-r-1]+diffWminSbox*roundActiveSboxNum[r-1])<=(Bnc[round-1])){}
	//else return;
	
	if(r==round){
		if((roundProb[r-2]+diffWminSbox*roundActiveSboxNum[r-1])<=(Bnc[round-1])){}
		else return;
		searchRoundN(0,0,tmp1);//1是起始活跃S盒，0是起始概率
	}else{
		if((roundProb[r-2]+Bn[round-r-1]+diffWminSbox*roundActiveSboxNum[r-1])<=(Bnc[round-1])){}
		else return;
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
		memset(roundCharacteristic[0]+a_pre+1,0,sizeof(u8)*(a_cur-a_pre-1));
		for(si16 o=1;o<outNum;o++){
			prob=diffProb[ISDiffInputMaxProb[o]]+pr_round;
			if((prob+Bn[round-2])<=(Bnc[round-1])){
				roundCharacteristic[0][a_cur]=o;
				roundProb[0]=prob;
				memset(roundCharacteristic[0]+a_cur+1,0,sizeof(u8)*(sboxNum-1-a_cur));
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
	Bn[0]=diffWminSbox;
	Bn[1]=2*diffWminSbox;
	for(round=3;round<=rounds;round++){
		prType BnLowerBound=Bn[0]+Bn[round-2];
		for(int i=1;i<round/2;i++){
			prType tmp=Bn[i]+Bn[round-2-i];
			if(tmp>BnLowerBound){
				BnLowerBound=tmp;
			}
		}
		for(prType step=BnLowerBound;;step++){
			Bnc[round-1]=step;
			clock_t start,end;
			start=clock();
			searchRound1();
			end=clock();
			printf("round:%d Bnc:%d time:%f\n",round,Bnc[round-1],(double)(end-start)/CLK_TCK);
			if(trailCount[round-1]!=0){
				Bn[round-1]=Bnc[round-1];
				break;
			}
		}
	}
}

/*void PRESENT::traverseRound1(){
	__m128i tmp1;
	tmp1=_mm_setzero_si128();
	__m128i *odp;
	odp=(__m128i *)(roundCharacteristic[0]);
	getInfo(0,*odp);
	traverseRound1(1,tmp1);
}

void PRESENT::traverseRound1(int j,__m128i tmp0){
	si8 an=roundActiveSboxNum[round-1];
	si8 ai=roundActiveSboxIndex[round-1][j];
	si8 an_remain=an-j-1;
	prType prob;
	u16 idv=roundCharacteristic[round-1][ai];
	
	si8 s;
	si8 m;
	__m128i tmp1;
	__m128i *odp;
	odp=(__m128i *)(roundCharacteristic[0]);

	si8 pr=diffInputMaxProb[idv];
	s=diff_0_Offset[idv][pr][0];
	m=diff_0_Number[idv][pr];
	for(si16 k=s;k<s+m;k++){//遍历输出差分
		roundCharacteristic1[ai]=
		tmp1=_mm_xor_si128(tmp0,*(__m128i *)(SPTable[ai][idv][k]));
		if(an_remain==0){
			_mm_store_si128(odp,tmp1);
			foundOne();
		}else{
			traverseRound1(j+1 ,tmp1);
		}
	}
}*/