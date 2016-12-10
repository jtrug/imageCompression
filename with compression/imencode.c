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

void printA(int [8][8]);
void printAd(double [8][8]);
void multiply(double [8][8],double [8][8],double [8][8]);
void quantilize(double [8][8],double );
void zigit(int [8][8], int [64]);
void unzig(int [8][8], int [64]);
void encode(int [64],double [8][8], double [8][8], int , int, int );

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
	double ct[8][8]={			
{ 0.3536 , 0.4904 , 0.4169 , 0.4157 , 0.3536 , 0.2778 , 0.1913 , 0.0975},
{ 0.3536 , 0.4157 , 0.1913 ,-0.0975 ,-0.3536 ,-0.4904 ,-0.4619 ,-0.2778},
{ 0.3536 , 0.2778 ,-0.1913 ,-0.4904 ,-0.3536 , 0.0975 , 0.4619 , 0.4157},
{ 0.3536 , 0.0975 ,-0.4619 ,-0.2778 , 0.3536 , 0.4157 ,-0.1913 ,-0.4904},
{ 0.3536 ,-0.0975 ,-0.4169 , 0.2778 , 0.3536 ,-0.4157 ,-0.1913 , 0.4904},
{ 0.3536 ,-0.2778 ,-0.1913 , 0.4904 ,-0.3536 ,-0.0975 , 0.4619 ,-0.4157},
{0.3536 ,-0.4157 , 0.1913 , 0.0975 ,-0.3536  ,0.4904 ,-0.4619 , 0.2778},
{ 0.3536 ,-0.4904 , 0.4619 ,-0.4157 , 0.3536 ,-0.2778 , 0.1913 , -0.0975}
 		};
 		
 	struct ZigZag
    {
        int row, col;
    };
    struct ZigZag z[8*8] =
    {
        { 0, 0 }, { 1, 0 }, { 0, 1 }, { 0, 2 },
        { 1, 1 }, { 2, 0 }, { 3, 0 }, { 2, 1 },
        { 1, 2 }, { 0, 3 }, { 0, 4 }, { 1, 3 },
        { 2, 2 }, { 3, 1 }, { 4, 0 }, { 5, 0 },
        { 4, 1 }, { 3, 2 }, { 2, 3 }, { 1, 4 },
        { 0, 5 }, { 0, 6 }, { 1, 5 }, { 2, 4 },
        { 3, 3 }, { 4, 2 }, { 5, 1 }, { 6, 0 },
        { 7, 0 }, { 6, 1 }, { 5, 2 }, { 4, 3 },
        { 3, 4 }, { 2, 5 }, { 1, 6 }, { 0, 7 },
        { 1, 7 }, { 2, 6 }, { 3, 5 }, { 4, 4 },
        { 5, 3 }, { 6, 2 }, { 7, 1 }, { 7, 2 },
        { 6, 3 }, { 5, 4 }, { 4, 5 }, { 3, 6 },
        { 2, 7 }, { 3, 7 }, { 4, 6 }, { 5, 5 },
        { 6, 4 }, { 7, 3 }, { 7, 4 }, { 6, 5 },
        { 5, 6 }, { 4, 7 }, { 5, 7 }, { 6, 6 },
        { 7, 5 }, { 7, 6 }, { 6, 7 }, { 7, 7 },
    };
int main(int argc, char *argv[]) {
	if(!(argc==5)){
		printf("Improper usages needs to have four arguments input picture filename, output filename, and q factor and number of processes\n");
		return 0;	
	}
	char *input =  argv[1];
	char *output = argv[2];
	FILE *fp = fopen(output,"w+");
	double q = atof(argv[3]);
	int p = atoi(argv[4]);
	int row,col,t;

	image_ptr ptr = read_pnm(input, &row, &col, &t);
	image_ptr compressed = ptr;
	
	double qa[8][8];
	quantilize(qa,q);
	fprintf(fp, "%d %d %d %d $ ", row,col,(int)q,t);
	double zerocounter=0,numcounter = 0,numcount2=0;
	double x[8][8]= {{0}},ytmp[8][8]= {{0}},y[8][8]= {{0}};
	int z[8][8] = {{0}}; 
	for(int i=0;i<row;i=i+8){
		for(int j =0;j<col;j=j+8){
			
			for(int k=i;k<i+8;k++){
					for(int l =j;l<j+8;l++){
						x[k%8][l%8]=(double)ptr[k*col+l];
						
					}
			}
			multiply(ytmp,c,x);
			multiply(y,ytmp,ct);
			for(int a =0;a<8;a++){
       			 for(int b =0;b<8;b++){
       			 	
					z[a][b]=round(y[a][b]/qa[a][b]);
					if(z[a][b]!=0){
						numcounter++;
        			}
        			else{
						zerocounter++;
						numcounter++;	
        			}
        		}
		    }
		    int last=0;
		    int output[64];
		    zigit(z,output);
		    for(int ii =0 ;ii<64;ii++){
		    	if(output[ii]!=0){
		    	last=ii;
		    	}
		    }
		    /*int last=0;
		    int output[64];
		   	encode(output,qa,x,numcounter,zerocounter,last);*/
		    for(int jj =0;jj<last+1;jj++){
		    	fprintf(fp, "%d ",output[jj]);
		    }
		    fprintf(fp,"$ ");
		    		    
		}
	}
printf("Image dimensions: %d X %d\n",row,col);
printf("Block Size: 8\n");
printf("Quantization Parameter: %2.0f\n",q);
printf("Percent Zeros: %3.2f\n",zerocounter*100/numcounter);
printf("Estimated file size: %3.2f\n",((100-zerocounter*100/numcounter)/100)*row*col*8);

}
void encode(int output[64], double qa[8][8], double x[8][8], int numcount, int zerocount, int last){
			double ytmp[8][8]= {{0}},y[8][8]= {{0}};
			int z[8][8] ={{0}};
			int zcount = 0, numc=0, lastnn=0 ;
			multiply(ytmp,c,x);
			multiply(y,ytmp,ct);
			for(int a =0;a<8;a++){
       			 for(int b =0;b<8;b++){
       			 	
					z[a][b]=round(y[a][b]/qa[a][b]);
					if(z[a][b]!=0){
						numcount++;
        			}
        			else{
						zerocount++;
						numcount++;	
        			}
        		}
		    }
		    zigit(z,output);
		    for(int ii =0 ;ii<64;ii++){
		    	printf("%d ", output[ii]);
		    	if(output[ii]!=0){
		    	last=ii;
		    	}
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
void printA(int b[8][8]){
    for(int i =0;i<8;i++){
        for(int j =0;j<8;j++){
            printf("%3.3d ",b[i][j]);
        }
        printf("\n");
    }
            printf("\n");

}
void printAd(double b[8][8]){
    for(int i =0;i<8;i++){
        for(int j =0;j<8;j++){
            printf("%3.3f ",b[i][j]);
        }
        printf("\n");
    }
            printf("\n");

}
void quantilize(double quant[8][8],double qf){
	for(int i =0;i<8;i++){
		for(int j =0;j<8;j++){
				quant[i][j] = qarray[i][j]*qf;
		}
	}
}
void zigit(int input[8][8], int output[64]){
	for(int i = 0;i<64;i++){
		output[i]= input[z[i].row][z[i].col];		
	}
 
}
void unzig(int output[8][8], int input[64]){
	for(int i = 0;i<64;i++){
		output[z[i].col][z[i].row] = input[i];
	}
 
}





