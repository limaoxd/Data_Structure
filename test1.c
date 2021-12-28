#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SWAP(a,b)   ({ __typeof__(a) temp; temp = a; a = b; b = temp;})
#define MEM(i,j)    memset(i,j,sizeof i);
#define MAXN 2147483647

typedef long long ll;

int* call(int* a){
    *a = 10;
    int* b = malloc(sizeof(int));
    *b = 5;
    a=b;
    return a;
}

int main(){
    int* a = malloc(sizeof(int));
    a = call(a);
    if(a) printf("%d",*a);
}