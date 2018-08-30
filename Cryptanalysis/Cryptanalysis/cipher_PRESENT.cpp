#include "cipher_PRESENT.h"

PRESENT::PRESENT(){
	byte sboxArr[inNum]={0xC, 0x5, 0x6, 0xB , 0x9 , 0x0 , 0xA , 0xD , 0x3 , 0xE , 0xF , 0x8 , 0x4 , 0x7 , 0x1 , 0x2};
	memcpy(sbox,sboxArr,inNum);
	genDiffTable();
	genLinearTable();
	genISbox();

	int PArr[blockBits]={
0,16,32,48,1,17,33,49,2,18,34,50,3,19,35,51,
4,20,36,52,5,21,37,53,6,22,38,54,7,23,39,55,
8,24,40,56,9,25,41,57,10,26,42,58,11,27,43,59,
12,28,44,60,13,29,45,61,14,30,46,62,15,31,47,63};
	int iPArr[blockBits]={
0,4,8,12,16,20,24,28,32,36,40,44,48,52,56,60,
1,5,9,13,17,21,25,29,33,37,41,45,49,53,57,61,
2,6,10,14,18,22,26,30,34,38,42,46,50,54,58,62,
3,7,11,15,19,23,27,31,35,39,43,47,51,55,59,63};
	memcpy(PTable,PArr,sizeof(int)*blockBits);
	memcpy(iPTable,iPArr,sizeof(int)*blockBits);
	genPermTable();

	
	byte W[16] = {0x0, 0x1, 0x2, 0x4, 0x8, 0x3, 0x5, 0x6, 0x9, 0xa, 0xc, 0x7, 0xb, 0xd, 0xe, 0xf};
	memcpy(Wto,W,sizeof(byte)*16);
	genStatistics();
	genSPTable();

	memset(trailCount,0,sizeof(int)*(rounds+1));
}

void PRESENT::genDiffTable(){
	memset(diffTable,0,sizeof(int)*inNum*outNum);
	for(int i=0;i<inNum;i++){
		for(int d=0;d<inNum;d++){
			diffTable[d][sbox[i]^sbox[i^d]]++;
		}
	}
}

void PRESENT::genLinearTable(){
	memset(linearTable,0,sizeof(int)*inNum*outNum);
	for (byte ii=0x0; ii<inNum; ii++){
		byte oi = sbox[ii];
		for (byte il=0x0; il<inNum; il++){
			for (byte ol=0x0; ol<outNum; ol++){
				if (xorSum(ii&il,inBits) == xorSum(oi&ol,outBits)){
					linearTable[il][ol]++;
				}
			}
		}
	}

	for(byte il=0x0; il<inNum; il++){
		for(byte ol=0x0; ol<outNum; ol++){
			int temp=linearTable[il][ol];
			int mid=(inNum>>1);
			//linearTable[inNum*il+ol] = temp-mid;//有正负号
			linearTable[il][ol] = (temp>mid)?(temp-mid):(mid-temp);
		}
	}
}

void PRESENT::genISbox(){
	for (byte x = 0; x<inNum; x++)
	{
		isbox[sbox[x]] = x;
	}
}

void PRESENT::printSbox(){
	printf("S盒:\n");
	for(int i=0;i<inNum;i++){
		printf("%02x ",sbox[i]);
	}printf("\n");
	printf("S盒的逆:\n");
	for(int i=0;i<inNum;i++){
		printf("%02x ",isbox[i]);
	}printf("\n");
}

void PRESENT::printSboxDiffTable(){
	printf("差分分布表\n");
	for(int i=0;i<inNum;i++){
		for(int j=0;j<outNum;j++){
			printf("%d ",diffTable[i][j]);
		}printf("\n");
	}printf("\n");
}

void PRESENT::printSboxLinearTable(){
	printf("线性分布表\n");
	for(int i=0;i<inNum;i++){
		for(int j=0;j<outNum;j++){
			printf("%d ",linearTable[i][j]);
		}printf("\n");
	}printf("\n");
}

void PRESENT::P(byte *Pod,byte *Pid){
	memset(Pod,0,sboxNum);
	int n;
	for(int m=0;m<blockBits;m++){
		n=PTable[m];
		Pod[n/inBits]|=( ((Pid[m/inBits]>>(inBits-1-m%inBits))&0x1) << (inBits-1-n%inBits) );
	}
}

void PRESENT::iP(byte *Pod,byte *Pid){
	memset(Pod,0,sboxNum);
	int n=0;
	for(int m=0;m<blockBits;m++){
		n=iPTable[m];
		Pod[n/inBits]|=( ((Pid[m/inBits]>>(inBits-1-m%inBits))&0x1) << (inBits-1-n%inBits) );
	}
}

void PRESENT::genPermTable(){
	ALIGNED_TYPE_(byte,16) Pid[sboxNum],Pod[sboxNum];
	for(int si=0;si<sboxNum;si++){
		memset(Pid,0,sboxNum);
		memset(Pod,0,sboxNum);
		for(int ii=0;ii<inNum;ii++){
			Pid[si]=ii;
			P(Pod,Pid);
			memcpy(permTable[si][ii],Pod,sboxNum);
		}
	}
}

void PRESENT::genStatistics(){
	diff_Number=0;
	memset(diff_1_Number,0,diffProbNum*sizeof(int));
	memset(diff_0_Number,0,diffProbNum*outNum*sizeof(int));

	for(int i=0;i<inNum;i++){
		for(int o=0;o<outNum;o++){
			if(diffTable[i][o]!=0&&diffTable[i][o]!=inNum){
				diff_Number++;
				diff_1_Number[(diffProbNum-diffTable[i][o]/2)]++;
				diff_0_Number[i][(diffProbNum-diffTable[i][o]/2)]++;
			}
		}
		diff_0_Offset[i][0][0]=0;
		diff_0_Offset[i][0][1]=diff_0_Number[i][0];
		for(int p=1;p<diffProbNum;p++){
			diff_0_Offset[i][p][0]=diff_0_Offset[i][p-1][1];
			diff_0_Offset[i][p][1]=diff_0_Offset[i][p-1][1]+diff_0_Number[i][p];
		}
	}
	diff_1_Offset[0][0]=0;
	diff_1_Offset[0][1]=diff_1_Number[0];
	for(int p=1;p<diffProbNum;p++){
		diff_1_Offset[p][0]=diff_1_Offset[p-1][1];
		diff_1_Offset[p][1]=diff_1_Offset[p-1][1]+diff_1_Number[p];
	}

	
	for(int p=0;p<diffProbNum;p++){
		diff_1_Non0Num[p]=0;
	}
	for(int p=0;p<diffProbNum;p++){
		for(int i=0;i<outNum;i++){
			if(diff_0_Number[i][p]!=0){
				diff_1_Non0Val[p][diff_1_Non0Num[p]]=i;
				diff_1_Non0Num[p]++;
			}
		}
	}

	diffProb[0]=2;
	diffProb[1]=3;
	diffInputMaxProb[0]=0;
	diffOutputMaxProb[0]=0;
	for(int i=1;i<inNum;i++){
		for(int o=1;o<inNum;o++){
			if(diffTable[i][o]==2){
				diffInputMaxProb[i]=1;
				diffOutputMaxProb[o]=1;
			}
		}
	}
	for(int i=1;i<inNum;i++){
		for(int o=1;o<inNum;o++){
			if(diffTable[i][o]==4){
				diffInputMaxProb[i]=0;
				diffOutputMaxProb[o]=0;
			}
		}
	}
	/*for(int p=diffProbNum-1;p>=0;p--){
		for(int i=0;i<diff_1_Non0Num[p];i++){
			diffInputMaxProb[diff_1_Non0Val[p][i]]=p;
		}
	}*/
}

void PRESENT::genSPTable(){
	int diff_1_Count[diffProbNum];
	int diff_0_Count[diffProbNum];
	for(int p=0;p<diffProbNum;p++){
		diff_1_Count[p]=diff_1_Offset[p][0];
	}
	for(int i=0;i<inNum;i++){
		for(int p=0;p<diffProbNum;p++){
			diff_0_Count[p]=diff_0_Offset[Wto[i]][p][0];
		}
		for(int o=0;o<outNum;o++){
			if(diffTable[Wto[i]][Wto[o]]!=0&&diffTable[Wto[i]][Wto[o]]!=inNum){
				for(int si=0;si<sboxNum;si++){
					memcpy(SPTable[si][Wto[i]][diff_0_Count[diffProbNum-diffTable[Wto[i]][Wto[o]]/2]],permTable[si][Wto[o]],sboxNum*sizeof(byte));
				}
				diff_1_Count[diffProbNum-diffTable[Wto[i]][Wto[o]]/2]++;
				diff_0_Count[diffProbNum-diffTable[Wto[i]][Wto[o]]/2]++;
			}
		}
	}
}

void PRESENT::fprintStatistics(){
	FILE* fp;
	fp=fopen("statistics.txt","w");
	fprintf(fp,"diff_0_Offset:\n");
	for(int i=0x0;i<inNum;i++){
		fprintf(fp,"*/0x%02x*/",i);
		for(int p=0x0;p<diffProbNum;p++){
			fprintf(fp,"%d:%d-(%d,%d)\t",p,diff_0_Number[i][p],diff_0_Offset[i][p][0],diff_0_Offset[i][p][1]);
		}
		fprintf(fp,"\n");
	}
	fprintf(fp,"diff_1_Offset:\n");
	for(int p=0x0;p<diffProbNum;p++){
		fprintf(fp,"%d:%d-(%d,%d)\t",p,diff_1_Number[p],diff_1_Offset[p][0],diff_1_Offset[p][1]);
	}
	fprintf(fp,"\n");

	fprintf(fp,"diff_1_Non0:\n");
	for(int p=0;p<diffProbNum;p++){
		fprintf(fp,"%d:%d\n",p,diff_1_Non0Num[p]);
		for(int i=0;i<diff_1_Non0Num[p];i++){
			fprintf(fp,"%02x\t",diff_1_Non0Val[p][i]);
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
		fprintf(fp,"%d\t",diffInputMaxProb[i]);
	}
	fprintf(fp,"\n");
	fprintf(fp,"diffOutputMaxProb:\n");
	for(int o=0;o<outNum;o++){
		fprintf(fp,"%d\t",diffOutputMaxProb[o]);
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
				for(int k=diff_0_Offset[i][p][0];k<diff_0_Offset[i][p][1];k++){
					fprintf(fp,"(");
					for(int l=0;l<16;l++){
						fprintf(fp,"%02x,",SPTable[si][i][k][l]);
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
				fprintf(fp,"0x%02x,",permTable[s][id][i]);
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
	trailCount[round-1]++;
}

void PRESENT::foundOne(){
	Bnc[round-1]=roundProb[round-1];
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

	for(si16 pr=0;pr<diffProbNum;pr++){//遍历概率
		prob=diffProb[pr]+pr_round;
		if((prob+roundProb[r-2]+Bn[round-r-1]+diffWminSbox*an_remain)<=(Bnc[round-1])){
			s=diff_0_Offset[idv][pr][0];
			m=diff_0_Number[idv][pr];
			for(si16 k=s;k<s+m;k++){//遍历输出差分
				tmp1=_mm_xor_si128(tmp0,*(__m128i *)(SPTable[ai][idv][k]));
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

	si8 pr=diffInputMaxProb[idv];
	prob=diffProb[pr]+pr_round;
	if((prob+roundProb[round-2]+diffWminSbox*an_remain)<=(Bnc[round-1])){
		s=diff_0_Offset[idv][pr][0];
		m=diff_0_Number[idv][pr];
		for(si16 k=s;k<s+m;k++){//遍历输出差分
			tmp1=_mm_xor_si128(tmp0,*(__m128i *)(SPTable[ai][idv][k]));
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
			prob=diffProb[diffOutputMaxProb[o]]+pr_round;
			if((prob+Bn[round-2])<=(Bnc[round-1])){
				roundCharacteristic[0][a_cur]=o;
				roundProb[0]=prob;
				memset(roundCharacteristic[0]+a_cur+1,0,sizeof(u8)*(sboxNum-1-a_cur));
				tmp1=_mm_xor_si128(tmp0,*(__m128i *)(permTable[a_cur][o]));
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