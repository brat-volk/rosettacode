#include <stdio.h>
#include <math.h>

//#define DEBUG
#define upper_exponent 7

void main(void){
    freopen("output.txt","w",stdout);
    printf("j:0,i:1  0\n");
    for(int i=1;i<=upper_exponent;i++)
        for(int j=(int)pow(10,i-1);j<(int)pow(10,i);j++)
            #ifndef DEBUG
            if(narcissistExt(j,i)!=-1)
            #endif
                printf("j:%d,i:%d  %d\n",j,i,narcissistExt(j,i));
}

int narcissistExt(int n, int e){
    int s=0,t=e;
    for(;e>=0;e--)
        s+=(int)pow(digit(n,e),t);
    return s==n?s:-1;
}

int digit(int n,int p){
    return n/(int)pow(10,p)%10;
}
