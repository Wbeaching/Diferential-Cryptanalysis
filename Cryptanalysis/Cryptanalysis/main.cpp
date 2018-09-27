#include "cipher_PRESENT.h"
#include "cipher_DES.h"


struct characteristic{
	ALIGNED_TYPE_(byte,16) rc[32][16];
};

int characteristicXOR(characteristic cha1,characteristic cha2,int r){
	__m128i *add1;
	__m128i *add2;
	__m128i tmp;
	int weight1;
	int weight2;
	int weight=0;
	for(int i=0;i<=r;i++){
		add1=(__m128i *)(cha1.rc[i]);
		add2=(__m128i *)(cha2.rc[i]);
		tmp=_mm_and_si128(*add1,*add2);
		unsigned __int64 *val = (unsigned __int64*) &tmp;
		weight1=_mm_popcnt_u64(val[0]);
		weight2=_mm_popcnt_u64(val[1]);
		weight=weight+weight1+weight2;
	}
	return weight&1;
}

#define CIPHER_DES 1
#define CIPHER_PRESENT 0
int main(){
#if CIPHER_PRESENT
	PRESENT present;
	present.searchForBestDiffTrails();

	//byte pt[16]={7,0,0,0,0,0,0,0,0,0,0,0,7,0,0,0};//3
	//byte ct[16]={0,0,0,0,1,0,1,1,0,0,0,0,0,0,0,0};//3
	//byte pt[16]={0,0,0,0,0,0,0,0,0,0,0,0,0xd,0,0,0xd};//5
	//byte ct[16]={0,0,0,0,4,4,4,0,0,0,0,0,0,4,0,0};//5
	//byte pt[16]={0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0};//6
	//byte ct[16]={0,0,0,0,4,4,0,0,0,0,0,0,4,4,0,0};//6
	/*
	byte pt[16]={0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0};//5
	byte ct[16]={0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0};//5
	int b;
	for(int r=5;r<6;r++){
		for(b=present.Bn[r];;b++){
			present.searchForCluster(pt,ct,b,r);
			if(present.trailCount[r]!=0)break;
		}
	}
	
	vector<characteristic> characteristicSet;
	ifstream fin("5-round_clusterTrails.txt");
	characteristic characteristicTmp;
	for(int num=0;num<present.trailCount[5];num++){
		for(int i=0;i<=5;i++){
			for(int s=0;s<16;s++){
				fin>>characteristicTmp.rc[i][s];
			}
		}
		characteristicSet.push_back(characteristicTmp);
	}

	FILE *fp;
	fp=fopen("data.txt","w");
	srand((unsigned)time(NULL));
	for(int k=0;k<5000;k++){	
		byte key[20];
		ALIGNED_TYPE_(byte,16) roundKey[32][16];
		for(int i=0;i<20;i++){
			key[i]=rand();
			printf("%d ",key[i]);
		}printf("\n");
		present.generateRoundKeys(key,roundKey);
		characteristic characteristicKey;
		for(int i=0;i<=5;i++){
			for(int s=0;s<16;s++){
				characteristicKey.rc[i][s]=roundKey[i][s];
			}
		}

		int ltotal=0;
		for(int i=0;i<characteristicSet.size();i++){
			int l=characteristicXOR(characteristicSet[i],characteristicKey,5);
			ltotal=l==1?ltotal+1:ltotal-1;
		}
		fprintf(fp,"%d\n",ltotal>0?ltotal:-ltotal);
	}
	fclose(fp);*/
#endif

#if CIPHER_DES
	DES des;
#endif
	system("pause");
	return 0;
}
