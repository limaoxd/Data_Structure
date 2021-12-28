#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SWAP(a,b)   ({ __typeof__(a) temp; temp = a; a = b; b = temp;})
#define MEM(i,j)    memset(i,j,sizeof i);
#define MAXN 2147483647

typedef long long ll;

int main (){
    char c[512];
    ll val;
    while(scanf("%s",c)!=EOF){
        if(strcmp(c,"insert")==0){
            scanf("%lld",&val);
            insert(val);
        }
        else if(strcmp(c,"delete")==0){

        }
        else if(strcmp(c,"show")==0){
 
        }
        else break;
    }
    return 0;
}