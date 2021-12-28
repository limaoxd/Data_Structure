#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>

#define MEM(i,j)    memset(i,j,sizeof i);
#define X           first
#define Y           second
#define pb          push_back
#define mp          make_pair

typedef             long long ll;
const double Eps  = 1e-8;
const ll Inf      = 1e18+7;
const int Mod     = 1e9+7;

struct Node
{
    ll degree;
    ll val;
    struct Node* next[13];
};

ll max_size = 0;
struct Node* min = NULL;

typedef struct{
    struct LNode* root; 
}Ltree;

struct LNode{
    int val;
    int s;
    struct LNode* l;
    struct LNode* r;
};

struct LNode* merge(struct LNode* a,struct LNode* b){
    if(!a) return b;
    if(!b) return a;

    if(a->val > b->val){
        struct LNode* temp = a;
        a = b;
        b = temp;
    }

    a->r = merge(a->r,b);
    if(!a->l){
        a->l = a-> r;
        a->r = NULL;
    }else{
        if(a->l->s < a->r->s){
            struct LNode* temp = a->l;
            a->l = a->r;
            a->r = temp;
        }
        a->s = (a->r->s)+1;
    }
    return a;
}

struct LNode* Remove(struct LNode* node){
    if(!node) return NULL;
    struct LNode* l = node->l;
    struct LNode* r = node->r;
    node->l =NULL;
    node->r =NULL;
    free(node);
    return merge(l,r);
}

void Bheap_add(struct Node* a,struct Node* b){
    a -> next[a -> degree++] = b;
}

struct Node* Bheap_merge(struct Node** list,struct Node* a,struct Node* b, int rank){
    a->val<b->val ? Bheap_add(a,b) : Bheap_add(b,a);
     
    list[rank] = NULL;
    return (a->val<b->val?a:b);
}

void Bheap_insert(struct Node** list,ll val){
    struct Node* new = malloc(sizeof(struct Node));
    new -> degree = 0;
    new -> val = val;

    struct Node* combined = new;

    for(int i=0;i<=max_size;i++){
        if(list[i] == NULL){
            list[i] = combined;
            break;
        }else{
            if(i == max_size) 
                max_size++;
            combined = Bheap_merge(list,combined,list[i],i);
            list[i] = NULL;
        }
        if(!min || min->val >= combined->val)
            min = combined;
    }
}
void Bheap_remove(struct Node** list){
    if(!min) return;

    if(min->degree == max_size) max_size--;
    list[min->degree] = NULL;
    for(int i=0;i<min->degree;i++){
        struct Node* combined = min->next[i];
        for(int j=combined->degree;j<=max_size;j++){
            if(list[j] == NULL){
                list[j] = combined;
                break;
            }else{
                if(i == max_size) 
                    max_size++;
                combined = Bheap_merge(list,combined,list[j],j);
                list[i] = NULL;
            }
        }
    }
    min = NULL;
    for(int i=0;i<=max_size;i++){
        if(!list[i]) continue;
        if(!min|| min->val >= list[i]->val)
            min = list[i];
    }
}

int main(){
    int n[7] = {100, 500, 1000, 2000, 3000, 4000, 5000};
    int t = 0;
    srand(time(NULL));
    while(t<7){
        time_t start,end;

        struct Node* list[13];
        Ltree tree={NULL};
        int num = n[t];

        int rand_list[num];

        //Initialize list
        for(int i=0;i<num;i++)
            rand_list[i] = rand()%INT_MAX;

        int randed[5000] = {0};
        int randed_in[5000] = {0};

        
        for(int i=0;i<num;i++)
            Bheap_insert(list,rand_list[i]);
        for(int i=0;i<5000;i++)
            randed_in[i] = rand()%INT_MAX;

        start = clock();
        for(int i=0;i<5000;i++){
            randed[i] = rand()%2;
            if(randed[i]){
                Bheap_insert(list,randed_in[i]);
            }
            else{
                Bheap_remove(list);
            }
        }
        end = clock();

        double t1 = difftime(end,start);
        
        for(int i=0;i<num;i++){
            struct LNode* new = (struct LNode*) malloc(sizeof(struct LNode));
            new->val = rand_list[i];
            new->s = 0;
            new->l = NULL;
            new->r = NULL;
            tree.root = merge(tree.root,new);
        }

        start = clock();
        for(int i=0;i<5000;i++){
            if(randed[i]){
                struct LNode* new = (struct LNode*) malloc(sizeof(struct LNode));
                new->val = randed_in[i];
                new->s = 0;
                new->l = NULL;
                new->r = NULL;
                tree.root = merge(tree.root,new);
            }
            else{
                tree.root = Remove(tree.root);
            }
        }
        end = clock();
        double t2 = difftime(end,start);
        free(tree.root);
        for(int i=0;i<13;i++){
            if(!list[i]) continue;
            free(list[i]);
        }
        min = NULL;
        max_size = 0;

        printf("%d %.4lf %.4lf\n",num,t1,t2);

        t++;
    }

    return 0;
}