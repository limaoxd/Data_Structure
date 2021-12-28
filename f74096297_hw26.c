#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SWAP(a,b)   ({ __typeof__(a) temp; temp = a; a = b; b = temp;})
#define MEM(i,j)    memset(i,j,sizeof i);
const int MAXN = 2147483647;

typedef long long ll;

int arr[1000006];
int size=0;

typedef struct{
    int X,Y;
} pair;

struct node{
    struct node *l,*r;
    int pos[2];
    int left,right;
};

struct node* root = NULL;

int cmp(const void *a, const void *b)
{
      int c = *((int*)a);
      int d = *((int*)b);
      if(c < d) return -1;
      else if (c == d) return 0;
      else return 1;
}

struct node* add(int l,int r){
    struct node* now = malloc(sizeof(struct node));
    
    now->pos[0] = l;
    now->pos[1] = r;
    now->left = arr[l];
    now->right = arr[r];
    //printf("%d %d\n",now->left,now->right);
    if(l >= r) return now;
    int mid = (l+r)/2;
    now->l = add(l,mid);
    now->r = add(mid+1,r);
    return now;
}

void build(){
    if(!size) return;
    qsort(arr,size,sizeof(int),cmp);
    root = add(0,size-1);
}

pair search(struct node* now,int l,int r){ 
    pair ans={0,-1};
    if(!now) return ans;
    if(now->left>=now->right){
        ans.X = now->pos[0];
        ans.Y = now->pos[1];
        return ans;
    }
    if(l<now->left) l = now->left;
    if(l>now->right) r = now->right;
    int mid = (now->left+now->right)/2;
    if(r<=mid) ans = search(now->l,l,r);
    else if(l>=mid) ans = search(now->r,l,r);
    else{
        ans.X = search(now->l,l,mid).X;
        ans.Y = search(now->r,mid+1,r).Y;
    }
    return ans;
}

int main (){
    char c[512];
    int val;
    int f = 0;
    int t;
    scanf("%d",&t);
    while(t--){
        scanf("%s",c);
        if(strcmp(c,"add")==0){
            scanf("%d",&val);
            if(f&&size>0) qsort(arr,size,sizeof(int),cmp);
            arr[size++] = val;
            if(root) free(root);
            root = NULL;
            f=0;
        }
        else if(strcmp(c,"remove")==0){
            scanf("%d",&val);
            if(!root) build();
            if(root&&size>0){
                pair pos = search(root,val,val);
                arr[pos.X] = MAXN;
                SWAP(arr[pos.X],arr[size--]);
                if(size <=0) {size =0;}
                if(size) f=1;
            }else f=0;
        }
        else if(strcmp(c,"range-search")==0){
            int l,r;
            scanf("%d %d",&l,&r);
            if(!root) build();

            pair pos = search(root,l,r-1);
            printf("range [%d,%d) = [",l,r);
            for(int i=pos.X;r>l&&i<=pos.Y;i++){
                if(i==pos.Y){
                    if(arr[i]<=r&&arr[i]>=l) printf("%d",arr[i]);
                }else{
                    if(arr[i]<=r&&arr[i]>=l) {
                        if(i+1<=pos.Y && arr[i+1]>r) printf("%d",arr[i]);
                        else printf("%d,",arr[i]);
                    }
                }
            }
            printf("]\n");
        }
        else break;
    }
    return 0;
}