#include "cipher_PRESENT.h"
#include <math.h>
int xorsum(byte x){
	int count=0;
	for(int i=0;i<4;i++){
		count+=((x>>i)&0x1);
	}
	return count;
}

int main(){
	PRESENT present;
	present.printSbox();
	present.printSboxDiffTable();
	present.printSboxLinearTable();
	present.fprintPermTable();
	present.fprintStatistics();
	present.fprintSPTable();
	present.searchForBestDiffTrails();
	system("pause");
	return 0;
}
