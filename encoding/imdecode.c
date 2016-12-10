#include <stdio.h>
#include "iplib2New.c"
#include <stdlib.h>
#include <string.h>
#include <math.h>

void quantilize(double [8][8],double );
void multiply(double [8][8],double [8][8],double [8][8]);
void printA(double [8][8]);

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
printf("%d %d %3.3f\n", t,col,q);
int values[size];
memset(values,0,size*sizeof(int));
unsigned char comp[size];
memset(comp,0,size*sizeof(unsigned char));
for (i = 4; i < ns; i=i+3){
	values[atoi(res[i])*col+atoi(res[i+1])]=(atoi(res[i+2]));
	if(atoi(res[i])==510 && atoi(res[i+1])==510){
		printf("%d\n", values[510*col+510]);
	}
}


quantilize(qarray,q);

double y[8][8],x[8][8],ytmp[8][8];
for(int i=0;i<row;i=i+8){
		for(int j =0;j<col;j=j+8){	
			for(int k=i;k<i+8;k++){
					for(int l =j;l<j+8;l++){
						y[k%8][l%8]=((double)values[k*col+l])*qarray[k%8][l%8];
					}
			}
			if(i==504&&j==504){printA(y);}
			multiply(ytmp,y,c);
			if(i==504&&j==504){printA(ytmp);}
			multiply(x,ct,ytmp);
			if(i==504&&j==504){printA(x);}

			for(int a =0;a<8;a++){
       			 for(int b =0;b<8;b++){
						comp[(i+a)*col+(j+b)] = (char)(x[a][b]);
        		}
		    }
		    
			
		}
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