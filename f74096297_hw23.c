#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SWAP(a,b)   ({ __typeof__(a) temp; temp = a; a = b; b = temp;})
#define MEM(i,j)    memset(i,j,sizeof i);
#define MAXN 2147483647

typedef long long ll;

typedef struct{
    int size;
    int node[500005];
}SMMH;

SMMH heap = {0};

void down_sift(int ind);

void up_sift (int ind){
    if(ind == 0 || ind == 1) return;
    int odd = ind&1;
    int parent_odd = ((ind-2)/2)&1;
    int parent = (ind-(2*(parent_odd+1)+odd))/2+odd;

    if(!odd && heap.node[ind] < heap.node[parent]) {
        SWAP(heap.node[ind],heap.node[parent]);
        up_sift(parent);
    }
    else if(odd && heap.node[ind] > heap.node[parent]) {
        SWAP(heap.node[ind],heap.node[parent]);
        up_sift(parent);
    }
    if(odd && heap.node[ind] < heap.node[ind-1]) {
        SWAP(heap.node[ind],heap.node[ind-1]);
        up_sift(ind-1);
        up_sift(ind);
    }
}

void down_sift(int ind){
    int size = heap.size;
    int odd = ind&1;
    int l = 2*ind+2-odd;
    int r = 2*ind+4-odd;

    if(!odd){
        if( r >= size && l < size&& heap.node[ind] > heap.node[l] || l<size && heap.node[ind] > heap.node[l] && heap.node[l] < heap.node[r]){
            SWAP(heap.node[ind],heap.node[l]);
            down_sift(l);
        }
        else if(r < size && heap.node[ind] > heap.node[r]){
            SWAP(heap.node[ind],heap.node[r]);
            down_sift(r);
        }
        if(ind +1 <heap.size&& heap.node[ind] > heap.node[ind+1]) {
            SWAP(heap.node[ind],heap.node[ind+1]);
            up_sift(ind+1);
            up_sift(ind);
        }
    }else{
        if(r >= size && l < size&& heap.node[ind] < heap.node[l] || l < size && heap.node[ind] < heap.node[l] && heap.node[l] > heap.node[r]){
            SWAP(heap.node[ind],heap.node[l]);
            down_sift(l);
        }
        else if(r < size && heap.node[ind] < heap.node[r]){
            SWAP(heap.node[ind],heap.node[r]);
            down_sift(r);
        }
        if(heap.node[ind] < heap.node[ind-1]) {
            SWAP(heap.node[ind],heap.node[ind-1]);
            up_sift(ind);
            up_sift(ind-1);
        }
    }
}

void insert (int val){
    int size = heap.size++;
    int odd = size&1;
    heap.node[size] = val;
    up_sift(size);
}

void delete (int mod){
    heap.size--;
    
    SWAP(heap.node[mod],heap.node[heap.size]);
    down_sift(mod);
}

void show (){
    printf("NULL");
    int level = 0,pow = 2;
    for(int i=0;i<heap.size;i++){
        if(i == level){
            printf("\n");
            level += pow ;
            pow *= 2;
        }
        printf("%d ",heap.node[i]);
    }
    printf("\n");
}

int main (){
    char c[512];
    ll val,offset;
    while(scanf("%s",c)!=EOF){
        if(strcmp(c,"insert")==0){
            scanf("%lld",&val);
            insert(val);
        }
        else if(strcmp(c,"delete")==0){
            scanf("%s",c);
            strcmp(c,"min")==0 ? delete(0) : delete(1);
        }
        else if(strcmp(c,"show")==0){
            show();
        }
        else break;
    }
    return 0;
}