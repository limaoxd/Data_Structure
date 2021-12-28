#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SWAP(a,b)   ({ __typeof__(a) temp; temp = a; a = b; b = temp;})
#define MEM(i,j)    memset(i,j,sizeof i);
#define MAXN 17
typedef long long ll;

struct Node
{
    ll deg;
    ll key;
    ll val;
    struct Node* prev;
    struct Node* next[MAXN];
};

struct Element
{
    struct Node* at;
    struct Element* prev;
    struct Element* next;
};

typedef struct{
    struct Element* begin;
    struct Element* back;
}List;

List list;
struct Node* Tree[MAXN];
struct Node* Min = NULL;

//Linked list
struct Element* find(ll key,ll val){
    struct Element* ind = list.begin;
    while (ind){
        if(ind->at->key == key && ind->at->val == val) return ind;
        ind = ind->next;
    }
    return NULL;
}

void pb(struct Node* a){
    struct Element* New = malloc(sizeof(struct Element));
    New->at = a;

    if(!list.begin){    
        list.begin = New;
        list.back = New;
    }
    else{
        list.back->next = New;
        New->prev = list.back;
        list.back = New;
    }
}

void Remove(struct Element* ind){
    if(ind->prev) ind->prev->next = ind->next;
    else list.begin = ind->next;
    if(ind->next) ind->next->prev = ind->prev;
    else list.back = ind->prev;
}
//F-heap

struct Node* merge(struct Node* a,struct Node* b){
    Tree[a -> deg] = NULL;
    if(a->val > b->val) SWAP(a,b);

    a -> next[a -> deg++] = b;
    b -> prev = a;
    return a;
}

struct Node* carry(struct Node* c){
    for(ll i=c->deg;i<MAXN;i++){
        if(!Tree[i]) return (Tree[i] = c);
        c = merge(c,Tree[i]);
    }
}

void insert(ll key,ll val){
    struct Node* New = malloc(sizeof(*New));
    *New = (struct Node) {.key = key,.val = val};
    pb(New);
    carry(New);
}

struct Node* sift(struct Node* c,ll offset){
    c->val -= offset;

    while(c->prev && c->val <= c-> prev -> val){
        SWAP(c->key,c->prev->key);
        SWAP(c->val,c->prev->val);
        c = c->prev;
    }
    return c;
}

struct Node* get_min(){
    Min = NULL;
    for(int i=0;i<MAXN;i++){
        if(!Tree[i]) continue;
        if(!Min|| Min->val > Tree[i]->val) Min = Tree[i];
    }
}

void cascading_cuts(struct Node* a){
    Tree[a->deg] = NULL;
    for(int i=0;i<a->deg;i++) carry(a->next[i]);
}

void decrease(ll key,ll val,ll offset){
    sift(find(key,val)->at,offset);
}

void delete(ll key,ll val){
    struct Node* delete_node = find(key,val)->at;
    delete_node = sift(delete_node,2147483647);
    cascading_cuts(delete_node);
    Remove(find(delete_node->key,delete_node->val));
}

void extract(){
    cascading_cuts(Min);
    Remove(find(Min->key,Min->val));
}

int main(){
    char c[512];
    ll key,val,offset;

    while(scanf("%s",c)!=EOF){
        if(strcmp(c,"insert")==0){
            scanf("%lld %lld",&val,&key);
            insert(key,val);
        }
        else if(strcmp(c,"delete")==0){
            scanf("%lld %lld",&val,&key);
            delete(key,val);
        }
        else if(strcmp(c,"decrease")==0){
            scanf("%lld %lld %lld",&val,&key,&offset);
            decrease(key,val,offset);
        }
        else if(strcmp(c,"extract")==0){
            if(!Min) continue;
            printf("(%lld)%lld\n",Min->val,Min->key);
            extract();
        }
        else    break;
        get_min();
    }
    return 0;
}