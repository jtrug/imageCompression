/*
 * imencode.c
 *
 *  Created on: Oct 18, 2015
 *      Author: jaredbartrug
 */
#include <stdio.h>
#include "iplib2New.c"
#include <stdlib.h>
#include <math.h>

void printA(double [8][8]);
void multiply(double [8][8],double [8][8],double [8][8]);

int main(int argc, char *argv[]) {
	char *input =  argv[1];
	char *output = argv[2];
	FILE *fp = fopen(output,"w+");
	double q = atof(argv[3]);
	int row,col,t;
	image_ptr ptr = read_pnm(argv[1], &row, &col, &t);
	int rowb = row/8;
	int colb = row/8;
	double c[8][8] = {
		{.3536,.3536,.3536,.3536,.3536,.3536,.3536,.3536},
		{.4904,.4157,.2778,.0975,-.0975,-.2778,-.4157,-.4904},
		{.4169,.1913,-.1913,-.4619,-.4169,-.1913,.1913,.4619},
		{.4157,-.0975,-.4904,-.2778,.2778,.4904,.0975,-.4157},
		{.3536,-.3536,-.3536,.3536,.3536,-.3536,-.3536,.3536},
		{.2778,-.4904,.0975,.4157,-.4157,-.0975,.4904,-.2778},
		{.1913,-.4619,.4619,-.1913,-.1913,.4619,-.4619,.1913},
		{.0975,-.2778,.4157,-.4904,.4904,-.4157,.2778,-.0975}
	};
	double ct[8][8];
	for(int i =0;i<8;i++){
        for(int j =0;j<8;j++){
			ct[i][j]=c[j][i];
        }
    }
	double qarray[8][8] = {
			{8,16,24,32,40,48,56,64},
			{16,24,32,40,48,56,64,72},
			{24,32,40,48,56,64,72,80},
			{32,40,48,56,64,72,80,88},
			{40,48,56,64,72,80,88,96},
			{48,56,64,72,80,88,96,104},
			{56,64,72,80,88,96,104,112},
			{64,72,80,88,96,104,112,120},
	};
	for(int i =0;i<8;i++){
		for(int j =0;j<8;j++){
				qarray[i][j] = qarray[i][j]*q;
		}
	}
	double zerocounter=0,numcounter = 0;
	double x[8][8],ytmp[8][8],y[8][8],z[8][8];
	fprintf(fp,"%d %d 8 8 %1.0f ",row,col,(double)q);
	for(int i=0;i<row;i=i+8){
		for(int j =0;j<col;j=j+8){
			for(int k=i;k<i+8;k++){
					for(int l =j;l<j+8;l++){
						x[k%8][l%8]=(double)ptr[k*col+l];
					}
			}
			multiply(ytmp,x,c);
			multiply(y,ytmp,ct);
			
			for(int a =0;a<8;a++){
       			 for(int b =0;b<8;b++){
					z[a][b]=round(y[a][b]/qarray[a][b]);
					if(z[a][b]!=0){
					fprintf(fp,"%d %d %3.0f ",(a+i),(b+j),z[a][b]);
					numcounter++;
        			}
        			else{
        				zerocounter++;	
        			}
        		}
		    }
		    
			
		}
	}
	printf("%3.3f",zerocounter/numcounter);
}
void printA(double b[8][8]){
    for(int i =0;i<8;i++){
        for(int j =0;j<8;j++){
            printf("%3.3f ",b[i][j]);
        }
        printf("\n");
    }
            printf("\n");

}
void multiply(double D[8][8],double A[8][8],double B[8][8]){
		for(int x=0; x<8; ++x)
 			for(int y=0; y<8; ++y){
 			D[x][y]=0.0;
				for(int z=0; z< 8; ++z)
					{						
						D[x][y] = D[x][y] + A[x][z]*B[z][y];
					} 
					}
}

