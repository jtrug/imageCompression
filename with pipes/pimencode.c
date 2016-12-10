/*
 * imencode.c
 *
 *  Created on: Oct 18, 2015
 *      Author: jaredbartrug
 */
#define BLOCK 8
#define _GNU_SOURCE

#include <stdio.h>
#include "iplib2New.c"
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
 #include <sys/wait.h>
#include <fcntl.h>


 void printA(int [BLOCK][BLOCK]);
 void printAd(double [BLOCK][BLOCK]);
 void multiply(double [BLOCK][BLOCK],double [BLOCK][BLOCK],double [BLOCK][BLOCK]);
 void quantilize(double );
 void zigit(int [BLOCK][BLOCK], int [BLOCK*BLOCK]);
 void unzig(int [BLOCK][BLOCK], int [BLOCK*BLOCK]);
 void encode(int [BLOCK*BLOCK], double [BLOCK][BLOCK], double* , double*, int* );
 void child(int, int, int, int, int , int, double [BLOCK][BLOCK]);
 void writevid(int , int , int , int , int , int [BLOCK*BLOCK]);

 double c[BLOCK][BLOCK] = {
 	{.3536,.3536,.3536,.3536,.3536,.3536,.3536,.3536},
 	{.4904,.4157,.2778,.0975,-.0975,-.2778,-.4157,-.4904},
 	{.4169,.1913,-.1913,-.4619,-.4169,-.1913,.1913,.4619},
 	{.4157,-.0975,-.4904,-.2778,.2778,.4904,.0975,-.4157},
 	{.3536,-.3536,-.3536,.3536,.3536,-.3536,-.3536,.3536},
 	{.2778,-.4904,.0975,.4157,-.4157,-.0975,.4904,-.2778},
 	{.1913,-.4619,.4619,-.1913,-.1913,.4619,-.4619,.1913},
 	{.0975,-.2778,.4157,-.4904,.4904,-.4157,.2778,-.0975}
 };
 double qarray[BLOCK][BLOCK] = {
 	{8,16,24,32,40,48,56,64},
 	{16,24,32,40,48,56,64,72},
 	{24,32,40,48,56,64,72,80},
 	{32,40,48,56,64,72,80,88},
 	{40,48,56,64,72,80,88,96},
 	{48,56,64,72,80,88,96,104},
 	{56,64,72,80,88,96,104,112},
 	{64,72,80,88,96,104,112,120},
 };
 double ct[BLOCK][BLOCK]={
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
 struct ZigZag z[BLOCK*BLOCK] =
 {
 	{ 0, 0 }, { 0, 1 }, { 1, 0 }, { 2, 0 },
 	{ 1, 1 }, { 0, 2 }, { 0, 3 }, { 1, 2 },
 	{ 2, 1 }, { 3, 0 }, { 4, 0 }, { 3, 1 },
 	{ 2, 2 }, { 1, 3 }, { 0, 4 }, { 0, 5 },
 	{ 1, 4 }, { 2, 3 }, { 3, 2 }, { 4, 1 },
 	{ 5, 0 }, { 6, 0 }, { 5, 1 }, { 4, 1 },
 	{ 3, 3 }, { 2, 4 }, { 1, 5 }, { 0, 6 },
 	{ 0, 7 }, { 1, 6 }, { 2, 5 }, { 3, 4 },
 	{ 4, 3 }, { 5, 2 }, { 6, 1 }, { 7, 0 },
 	{ 7, 1 }, { 6, 2 }, { 5, 3 }, { 4, 4 },
 	{ 3, 5 }, { 2, 6 }, { 1, 7 }, { 2, 7 },
 	{ 3, 6 }, { 4, 5 }, { 5, 4 }, { 6, 3 },
 	{ 7, 2 }, { 7, 3 }, { 6, 4 }, { 5, 5 },
 	{ 4, 6 }, { 3, 7 }, { 4, 7 }, { 5, 6 },
 	{ 6, 5 }, { 7, 4 }, { 7, 5 }, { 6, 6 },
 	{ 5, 7 }, { 6, 7 }, { 7, 6 }, { 7, 7 },
 };
 int row,col,t;

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

 	image_ptr ptr = read_pnm(input, &row, &col, &t);
 	image_ptr compressed = ptr;
 	int numBlocks = row/BLOCK *row/BLOCK;
 	int id[numBlocks],lastNum[numBlocks];
 	double pz[numBlocks], sz[numBlocks];
 	int value[numBlocks*BLOCK*BLOCK];
 	double qa[BLOCK][BLOCK];
 	int idp[2], lastNump[2],pzp[2],szp[2],valuep[2];
 	int **outputs;

 	pipe(idp);
 	pipe(lastNump);
 	pipe(pzp);
 	pipe(szp);
 	pipe(valuep);
 	fcntl(valuep[1], F_SETPIPE_SZ, 1048576);

 	quantilize(q);

 	double zerocounter=0,numcounter = 0,numcount2=0;
 	double x[numBlocks][BLOCK][BLOCK];
 	memset( x, 0, BLOCK*BLOCK*numBlocks*sizeof(double) );
 	int block=0;

 	while(block<numBlocks){
 		for(int a =0;a<BLOCK;a++){
 			for(int b =0;b<BLOCK;b++){
 				x[block][a][b]=(double)ptr[((block/(row/BLOCK))*BLOCK+a)*col+((block%(col/BLOCK))*BLOCK+b)];
 			}
 		}
 		block++;;
 	}
 	block = 0;
 	int itest[numBlocks];
 	double pvalues[p][(numBlocks/p) +1][BLOCK][BLOCK];
 	int pids[p][(numBlocks/p)+1];
 	while(block < numBlocks){
 		for(int ij = 0; ij<p; ij++){
 			if(block%p==ij){
 				pids[ij][block/p] = block;
 				for(int ik = 0; ik<BLOCK; ik++){
 					for(int il = 0; il<BLOCK; il++){
 						pvalues[ij][block/p][ik][il] = x[block][ik][il];
 					}
 				}
 				
 			}
 		}
 		block++;
 	}
 	block = 0;
 	int last=0;
 	int childn = 0;
 	for(int pr =0;pr<p;pr++){
 		pid_t pid = fork();
 		double numcounter2=0,zerocounter2=0;
 		int last2 = 0;
 		int output2[64] = {0};
 		if(pid==-1){

 		}
 		if(pid==0){
 			for(int u = 0; u<=numBlocks/p;u++){
 				numcounter2 = 0; 
 				zerocounter2 = 0;
 				last2 = 0;

 				encode(output2,pvalues[pr][u],&numcounter2,&zerocounter2,&last2);
 				double pz2 = zerocounter2*100/numcounter2;
 				double sz2 = ((100-pz2)/100)*row*col*BLOCK;
 				write(szp[1],&sz2, sizeof(sz2));
 				write(pzp[1],&pz2, sizeof(pz2));
 				int everything[BLOCK*BLOCK+2]={0};
 				everything[0] = pids[pr][u];
 				everything[1] = last2;
 				for(int j = 2; j<last2+3;j++){
 					everything[j] = output2[j-2];
 				}
 				write(lastNump[1],&last2, sizeof(last2));
 				int tmpid = pids[pr][u];
 				write(idp[1], &tmpid, sizeof(tmpid)); //Write the numbers as one array with the first one being block Id and second being last
 				for(int i =0;i<last2+3;i++){
 					int j = everything[i];
 					write(valuep[1], &j, sizeof(j));
 				}
 			}
 			exit(1);
 		}
 	}
 	wait(NULL);
 	wait(NULL);

 	close(szp[1]);
 	close(idp[1]);
 	close(pzp[1]);
 	close(lastNump[1]);
 	close(valuep[1]);
 	read(idp[0],id,numBlocks*sizeof(int));
 	read(pzp[0],pz,numBlocks*sizeof(double));
 	read(szp[0],sz,numBlocks*sizeof(double));
 	read(lastNump[0],lastNum,numBlocks*sizeof(int));
 	close(idp[0]);

 	int sum = 0;
 	double precent0 = 0;
 	for(int k =0;k<numBlocks;k++){
 		sum = sum +lastNum[k];
 		precent0 = precent0 + pz[k]*BLOCK*BLOCK/100;
 	}
 	read(valuep[0], value, sum*sizeof(int)+numBlocks*2*sizeof(int));
 	for(int v = 0;v<sum+numBlocks*2;v++){
 		fprintf(fp, "%d ", value[v]);
 	}
 	int data[numBlocks][64];
 	int lastnumber[numBlocks];
 	memset( data, 0, numBlocks*64*sizeof(int) );
 	int counter = 0;
 	double percentcount = 0;
 	int idcounter = 0;
 	int tid[numBlocks];
 	for(int m =0;m<sum+2*numBlocks;m++){
 		tid[counter] = value[m];
 		m++;
 		printf("%d %d %d\n",value[m-1] ,m,counter);

 		//printf("%d\n",m );
 		m = m + value[m]+2;
 		counter++;
 	}
 	 for(int m =0;m<numBlocks;m++){
 	 	//printf("%d\n",tid[m] );
 	 }

 	for(int m =0;m<numBlocks;m++){
 		tid[m] = value[idcounter];
 		idcounter++;
 		lastnumber[tid[m]] = value[idcounter];
 		for(int n =0;n<=lastnumber[tid[m]];n++){
 			idcounter++;
 			if(tid[m]<numBlocks && tid>=0 && value[idcounter]<257 && value[idcounter]>-257){
 				if(value[idcounter]!=0){
 					data[tid[m]][n]=value[idcounter];
 					percentcount++;
 				}
 			}
 		}
 		idcounter++;
 	}
 	fprintf(fp, "%d %d %d %d $ ", row, col, (int)q, t);
 	for(int nn = 0; nn <numBlocks;nn++){
 		for(int jj =0;jj<lastnumber[nn]+1;jj++){
 			fprintf(fp, "%d ",data[nn][jj]);

 		}
 		fprintf(fp,"$ ");
 	}
 	precent0 = 100-100*percentcount/(numBlocks*BLOCK*BLOCK);
 	printf("Image dimensions: %d X %d\n",row,col);
 	printf("Block Size: %d\n", BLOCK);
 	printf("Quantization Parameter: %2.0f\n",q);
 	printf("Percent Zeros: %3.2f\n",precent0);
 	printf("Estimated file size: %3.2f\n",((100-precent0)/100)*row*col*BLOCK);

 }
 void child(int szp, int pzp, int lastp, int valuep, int idp, int id, double values[BLOCK][BLOCK]){
 	double numcounter2=0,zerocounter2=0;
 	int last2 = 0;
 	int output2[64] = {0};
 	encode(output2,values,&numcounter2,&zerocounter2,&last2);
 	double pz2 = zerocounter2*100/numcounter2;
 	double sz2 = ((100-zerocounter2*100/numcounter2)/100)*row*col*BLOCK;
 	write(szp,&sz2, sizeof(sz2));
 	write(pzp,&pz2, sizeof(pz2));
 	write(lastp,&last2, sizeof(last2));
 	write(idp, &id, sizeof(id));
 	for(int i =0;i<last2+1;i++){
 		int j = output2[i];
 		write(valuep, &j, sizeof(output2[i]));
 	}
 }
 void writevid(int lastNump, int last2, int tmpid, int idp, int valuep, int output2[BLOCK*BLOCK]){
 	write(lastNump,&last2, sizeof(last2));
 	write(idp, &tmpid, sizeof(tmpid));
 	printf("%d\n",tmpid);
 	for(int i =0;i<last2+1;i++){
 		int j = output2[i];
 		printf("%d", j);
 		write(valuep, &j, sizeof(output2[i]));
 	}
 	printf("\n");
 }
 void encode(int output[BLOCK*BLOCK], double x[BLOCK][BLOCK], double * numcount, double * zerocount, int * last){
 	double ytmp[BLOCK][BLOCK]= {{0}},y[BLOCK][BLOCK]= {{0}};
 	int z[BLOCK][BLOCK] ={{0}};
 	int zcount = 0, numc=0, lastnn=0 ;
 	multiply(ytmp,c,x);
 	multiply(y,ytmp,ct);
 	for(int a =0;a<BLOCK;a++){
 		for(int b =0;b<BLOCK;b++){

 			z[a][b]=round(y[a][b]/qarray[a][b]);
 			if(z[a][b]!=0){
 				*numcount=*numcount+1;
 			}
 			else{
 				*zerocount=*zerocount+1;
 				*numcount=*numcount+1;
 			}
 		}
 	}
 	zigit(z,output);
 	for(int ii =0 ;ii<BLOCK*BLOCK;ii++){
 		if(output[ii]!=0){
 			*last=ii;
 		}
 	}
 }
 void multiply(double D[BLOCK][BLOCK],double A[BLOCK][BLOCK],double B[BLOCK][BLOCK]){
 	for(int x=0; x<BLOCK; ++x)
 		for(int y=0; y<BLOCK; ++y){
 			D[x][y]=0.0;
 			for(int z=0; z< BLOCK; ++z)
 			{
 				D[x][y] = D[x][y] + A[x][z]*B[z][y];
 			}
 		}
 	}
 	void printA(int b[BLOCK][BLOCK]){
 		for(int i =0;i<BLOCK;i++){
 			for(int j =0;j<BLOCK;j++){
 				printf("%3.3d ",b[i][j]);
 			}
 			printf("\n");
 		}
 		printf("\n");

 	}
 	void printAd(double b[BLOCK][BLOCK]){
 		for(int i =0;i<BLOCK;i++){
 			for(int j =0;j<BLOCK;j++){
 				printf("%3.3f ",b[i][j]);
 			}
 			printf("\n");
 		}
 		printf("\n");

 	}
 	void quantilize(double qf){
 		for(int i =0;i<BLOCK;i++){
 			for(int j =0;j<BLOCK;j++){
 				qarray[i][j] = qarray[i][j]*qf;
 			}
 		}
 	}
 	void zigit(int input[BLOCK][BLOCK], int output[BLOCK*BLOCK]){
 		for(int i = 0;i<BLOCK*BLOCK;i++){
 			output[i]= input[z[i].row][z[i].col];
 		}

 	}
 	void unzig(int output[BLOCK][BLOCK], int input[BLOCK*BLOCK]){
 		for(int i = 0;i<BLOCK*BLOCK;i++){
 			output[z[i].col][z[i].row] = input[i];
 		}

 	}
