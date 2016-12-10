#include <stdio.h>
#include "iplib2New.c"
#include <stdlib.h>
#include <math.h>

int main(int argc, char *argv[]) {
	char* original = argv[1];
	char* comp = argv[2];
	int rowO,colO,tO,colC,rowC,tC;
	image_ptr ptrO = read_pnm(original, &rowO, &colO, &tO);
	image_ptr ptrC = read_pnm(comp, &rowC, &colC, &tC);
	
	double mae= 0,rmae;
	
	for(int i = 0; i<rowO;i++){
		for(int j =0; j<colO;j++){
			mae = mae+abs(ptrO[i*colO+j]-ptrC[i*colO+j]);
		}
	}
	for(int i = 0; i<rowO;i++){
		for(int j =0; j<colO;j++){
			rmae = rmae+((ptrO[i*colO+j]-ptrC[i*colO+j])*(ptrO[i*colO+j]-ptrC[i*colO+j]));
		}
	}
	mae = mae/(rowO*colO);
	rmae = rmae/(rowO*colO);
	rmae = sqrt(rmae);
	printf("Mean Absolute Error: %3.3f\n",mae);
	printf("Root Mean Square Error %3.3f\n",rmae);
}