#include <stdio.h>
#include "iplib2New.c"
#include <stdlib.h>
#include <string.h>
#include <math.h>

void quantilize(double [8][8],double );
void multiply(double [8][8],double [8][8],double [8][8]);
void printA(double [8][8]);
void printAi(int [8][8]);
void unzig(double [8][8], double [64]);

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
	char* input = argv[1];
	char* output = argv[2];
	FILE *fp = fopen(input,"rb");
	long lSize;
	char *buffer;

	fseek( fp , 0L , SEEK_END);
	lSize = ftell( fp );
	rewind( fp );

	buffer = calloc( 1, lSize+1 );
	fread( buffer , lSize, 1 , fp);

	fclose(fp);
char ** res  = NULL;
char *  p    = strtok (buffer, " ");
int ns = 0, i;

while (p) {
  res = realloc (res, sizeof (char*) * ++ns);
  res[ns-1] = p;
  p = strtok (NULL, " ");
}
int row = atoi(res[0]);
int col = atoi(res[1]);
double q = atof(res[2]);
int t = atoi(res[3]);
int size = row*col;
int values[size];
memset(values,0,size*sizeof(int));
unsigned char comp[size];
memset(comp,0,size*sizeof(unsigned char));
double numbers[64]={0};
i = 5;
int block=0;
quantilize(qarray,q);
double y[8][8]={0},z[8][8]={0},ytmp[8][8]={0};
while(i<ns){
	int counter=0;
	for(int blah =0; blah<64;blah++){
		numbers[blah]=0;
	}
	while(res[i][0]!='$'){
		numbers[counter]=atof(res[i]);
		i++;
		counter++;
	}
	i++;
	for(int z1=0;z1<8;z1++){
		for(int z2=0;z2<8;z2++){
			y[z1][z2]=0;
			z[z1][z2]=0;
			ytmp[z1][z2]=0;
		}
	}
	unzig(y,numbers);
	for(int k=0;k<8;k++){
					for(int l=0;l<8;l++){
						y[k][l]=y[k][l]*qarray[k%8][l%8];
			}
	}
	multiply(ytmp,y,c);
	multiply(z,ct,ytmp);
	int a;
	int b;
	for(a =0;a<8;a++){
       	for(b =0;b<8;b++){
			comp[((block/(row/8))*8+a)*col+((block%(col/8))*8+b)] = (unsigned char)(z[a][b]);	
        }
	}
	block++;
}
write_pnm(comp,output,row,col,t);
}
void quantilize(double quant[8][8],double qf){
	for(int i =0;i<8;i++){
		for(int j =0;j<8;j++){
				quant[i][j] = qarray[i][j]*qf;
		}
	}
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
void printA(double b[8][8]){
    for(int i =0;i<8;i++){
        for(int j =0;j<8;j++){
            printf("%3.3f ",b[i][j]);
        }
        printf("\n");
    }
            printf("\n");

}
void unzig(double output[8][8], double input[64]){
	for(int i = 0;i<64;i++){
		output[z[i].row][z[i].col] = input[i];
	}
 
}
void printAi(int b[8][8]){
    for(int i =0;i<8;i++){
        for(int j =0;j<8;j++){
            printf("%3.3d ",b[i][j]);
        }
        printf("\n");
    }
            printf("\n");

}