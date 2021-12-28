#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRUE 1
#define FALSE 0
#define EMPTY 0

#define MAX(a, b)   ((a) > (b) ? (a) : (b))
#define NODE_ORDER		2 /*The degree of the tree.*/
#define NODE_POINTERS	(NODE_ORDER*2)
#define NODE_KEYS		NODE_POINTERS-1

typedef unsigned char bool;

typedef struct tree_node {
	int key_array[NODE_KEYS];
	struct tree_node *child_array[NODE_POINTERS];
	unsigned int key_index;
	bool leaf;
} node_t;

typedef struct {
	node_t *node_pointer;
	int key;
	bool found;
	unsigned int depth;
} result_t;

typedef struct {
	node_t *root;
	unsigned short order;
	bool lock;
} btree_t;

static int BTreeGetLeftMax(node_t *T);
static int BTreeGetRightMin(node_t *T);

static node_t *create_node()
{
	int i;
	node_t *new_node = (node_t *)malloc(sizeof(node_t));

	if(!new_node){
		printf("Out of memory");
		exit(0);
	}

	// Set Keys
	for(i = 0;i < NODE_KEYS; i++){
		new_node->key_array[i] = 0;
	}

	// Set ptr
	for(i = 0;i < NODE_POINTERS; i++){
		new_node->child_array[i] = NULL;
	}

	new_node->key_index = EMPTY;
	new_node->leaf = TRUE;

	return new_node;
}

btree_t *create_btree()
{
	btree_t *new_root = (btree_t *)malloc(sizeof(btree_t));

	if(!new_root){
		return NULL;
	}

	node_t *head = create_node();

	if(!head){
		return NULL;
	}

	new_root->order = NODE_ORDER;
	new_root->root = head;
	new_root->lock = FALSE;

	return new_root;
}

static result_t *get_resultset()
{
	result_t *ret = (result_t *)malloc(sizeof(result_t));

	if(!ret){
		exit(0);
	}

	ret->node_pointer = NULL;
	ret->key = 0;
	ret->found = FALSE;
	ret->depth = 0;

	return ret;
}

int k=0;
int ans = -1;
void find_rank(int rank, node_t *node){
    if (!node) return;
    for(int i=0;i<=node->key_index ;i++){
        find_rank(rank,node->child_array[i]);
        if(i+1<=node->key_index) {
            k++;
            if(k == rank){
                ans = node->key_array[i];
            }
        }
    }
}

result_t *search(int key, node_t *node)
{
	int i = 0;
    if(!node) {
        result_t *result = get_resultset();
		result->node_pointer = node;
		result->found = FALSE;
		return result;
    }
	while((i < node->key_index) && (key > node->key_array[i])){
		i++;
	}

	if(i == 4){
		i--;
	}

	if((i <= node->key_index) && (key == node->key_array[i])){
		result_t *result = get_resultset();
		result->node_pointer = node;
		result->key = i;
		result->found = TRUE;
		return result;
	}

	if(node->leaf){
		result_t *result = get_resultset();
		result->node_pointer = node;
		result->found = FALSE;
		return result;
	}else{
		result_t *result = get_resultset();
		return search(key, node->child_array[i]);
	}
}

static void split_child(node_t *parent_node, int i, node_t *child_array)
{
	int j;

	node_t *new_node = create_node();
	new_node->leaf = child_array->leaf;
	new_node->key_index = NODE_ORDER-1;

	for(j = 0;j < NODE_ORDER-1;j++){
		new_node->key_array[j] = child_array->key_array[NODE_ORDER+j];
	}

	if(child_array->leaf == 0){
		for(j = 0;j < NODE_ORDER;j++){
			new_node->child_array[j] = child_array->child_array[NODE_ORDER+j];
		}
	}
	child_array->key_index = NODE_ORDER-1;

	for(j = parent_node->key_index;j>=i;j--){
		parent_node->child_array[j+1] = parent_node->child_array[j];
	}

	parent_node->child_array[i] = new_node;

	for(j = parent_node->key_index;j>=i;j--){
		parent_node->key_array[j] = parent_node->key_array[j-1];
	}

	parent_node->key_array[i-1] = child_array->key_array[NODE_ORDER-1];

	parent_node->key_index++;
}
/* The BTreeInsertNonFull operation insert X into a non-full node T.before
** execute this operation,guarantee T is not a full node.
*/
static void insert_nonfull(node_t *n, int key){
	int i = n->key_index;
	
	if(n->leaf){
		// Shift until we fit
		while(i>=1 && key<n->key_array[i-1]){
			n->key_array[i] = n->key_array[i-1];
			i--;
		}
		n->key_array[i] = key;
		n->key_index++;
	}else{
		// Find the position i to insert.
		while(i>=1 && key<n->key_array[i-1]){
			i--;
		}
		//If T's ith child_array is full,split first.
		if(n->child_array[i]->key_index == NODE_KEYS){
			split_child(n, i+1, n->child_array[i]);
			if(key > n->key_array[i]){
				i++;
			}
		}
		//Recursive insert.
		insert_nonfull(n->child_array[i], key);
	}
}

node_t *insert(int key, btree_t *b)
{
	node_t *root = b->root;
	if(root->key_index == NODE_KEYS){ //If node root is full,split it.
		node_t *newNode = create_node();
		b->root = newNode; //Set the new node to T's Root.
		newNode->leaf = 0;
		newNode->key_index = 0;
		newNode->child_array[0] = root;
		split_child(newNode, 1, root);//Root is 1th child of newNode.
		insert_nonfull(newNode, key); //Insert X into non-full node.
	}else{ //If not full,just insert X in T.
		insert_nonfull(b->root, key);
	}

	return b->root;
}
/* The merge_children operation merge the root->K[index] and its two child
** and then set chlid1 to the new root.
*/
static void merge_children(node_t *root, int index, node_t *child1, node_t *child2){
	child1->key_index = NODE_KEYS;
	int i;
	//Move child2's key to child1's right half.
	for(i=NODE_ORDER;i<NODE_KEYS;i++)
		child1->key_array[i] = child2->key_array[i-NODE_ORDER];
	child1->key_array[NODE_ORDER-1] = root->key_array[index]; //Shift root->K[index] down.
	//If child2 is not a leaf node,must copy child2's [ptrchlid] to the new
	//root(child1)'s [child_array].
	if(0 == child2->leaf){
		for(i=NODE_ORDER;i<NODE_POINTERS;i++)
			child1->child_array[i] = child2->child_array[i-NODE_ORDER];
	}
	//Now update the root.
	for(i=index+1;i<root->key_index;i++){
		root->key_array[i-1] = root->key_array[i];
		root->child_array[i] = root->child_array[i+1];
	}
	root->key_index--;
	free(child2);
}
/* The BTreeBorrowFromLeft operation borrows a key from leftPtr.curPtr borrow
** a node from leftPtr.root->K[index] shift down to curPtr,shift leftPtr's
** right-max key up to root->K[index].
*/
static void BTreeBorrowFromLeft(node_t *root, int index, node_t *leftPtr, node_t *curPtr){
	curPtr->key_index++;
	int i;
	for(i=curPtr->key_index-1;i>0;i--)
		curPtr->key_array[i] = curPtr->key_array[i-1];
	curPtr->key_array[0] = root->key_array[index];
	root->key_array[index] = leftPtr->key_array[leftPtr->key_index-1];
	if(0 == leftPtr->leaf)
		for(i=curPtr->key_index;i>0;i--)
			curPtr->child_array[i] = curPtr->child_array[i-1];
	curPtr->child_array[0] = leftPtr->child_array[leftPtr->key_index];
	leftPtr->key_index--;
}
 
static void BTreeBorrowFromRight(node_t *root, int index, node_t *rightPtr, node_t *curPtr){
	curPtr->key_index++;
	curPtr->key_array[curPtr->key_index-1] = root->key_array[index];
	root->key_array[index] = rightPtr->key_array[0];
	int i;
	for(i=0;i<rightPtr->key_index-1;i++)
		rightPtr->key_array[i] = rightPtr->key_array[i+1];
	if(0 == rightPtr->leaf){
		curPtr->child_array[curPtr->key_index] = rightPtr->child_array[0];
		for(i=0;i<rightPtr->key_index;i++)
			rightPtr->child_array[i] = rightPtr->child_array[i+1];
	}
	rightPtr->key_index--;
}

static void BTreeDeleteNoNone(int X, node_t *root){
	int i;
	//Is root is a leaf node ,just delete it.
	if(1 == root->leaf){
		i=0;
		while( (i<root->key_index) && (X>root->key_array[i])) //Find the index of X.
			i++;
		//If exists or not.
		if(X == root->key_array[i]){
			for(;i<root->key_index-1;i++)
				root->key_array[i] = root->key_array[i+1];
			root->key_index--;
		}
		else{
			printf("Node not found.\n");
			return ;
		}
	}
	else{  //X is in a internal node.
		i = 0;
		node_t *prePtr = NULL, *nexPtr = NULL;
		//Find the index;
		while( (i<root->key_index) && (X>root->key_array[i]) )
			i++;
		if( (i<root->key_index) && (X == root->key_array[i]) ){ //Find it in this level.
			prePtr = root->child_array[i];
			nexPtr = root->child_array[i+1];
			/*If prePtr at least have d keys,replace X by X's precursor in
			 *prePtr*/
			if(prePtr->key_index > NODE_ORDER-1){
				int aPrecursor = BTreeGetLeftMax(prePtr);
				root->key_array[i] = aPrecursor;
				//Recursively delete aPrecursor in prePtr.
				BTreeDeleteNoNone(aPrecursor,prePtr);
			}
			else
			if(nexPtr->key_index > NODE_ORDER-1){
			/*If nexPtr at least have d keys,replace X by X's successor in
			 * nexPtr*/
				int aSuccessor = BTreeGetRightMin(nexPtr);
				root->key_array[i] = aSuccessor;
				BTreeDeleteNoNone(aSuccessor,nexPtr);
			}
			else{
			/*If both of root's two child have d-1 keys,then merge root->K[i]
			 * and prePtr nexPtr. Recursively delete X in the prePtr.*/
				merge_children(root,i,prePtr,nexPtr);
				BTreeDeleteNoNone(X,prePtr);
			}
		}
		else{ //Not find in this level,delete it in the next level.
			prePtr = root->child_array[i];
			node_t *leftBro = NULL;
			if(i<root->key_index)
				nexPtr = root->child_array[i+1];
			if(i>0)
				leftBro = root->child_array[i-1];
			/*root->child_array[i] need to borrow from or merge with his neighbor
			 * and then recursively delete. */
			if(NODE_ORDER-1 == prePtr->key_index){
			//If left-neighbor have at least d-1 keys,borrow.
				if( (leftBro != NULL) && (leftBro->key_index > NODE_ORDER-1))
					BTreeBorrowFromLeft(root,i-1,leftBro,prePtr);
				else //Borrow from right-neighbor
				if( (nexPtr != NULL) && (nexPtr->key_index > NODE_ORDER-1))
					BTreeBorrowFromRight(root,i,nexPtr,prePtr);
				//OR,merge with its neighbor.
				else if(leftBro != NULL){
					//Merge with left-neighbor
					merge_children(root,i-1,leftBro,prePtr);
					prePtr = leftBro;
				}
				else //Merge with right-neighbor
					merge_children(root,i,prePtr,nexPtr);
			}
			/*Now prePtr at least have d keys,just recursively delete X in
			 * prePtr*/
			BTreeDeleteNoNone(X,prePtr);
		}
	}
}
/*Get T's left-max key*/
static int BTreeGetLeftMax(node_t *T){
	if(0 == T->leaf){
		return BTreeGetLeftMax(T->child_array[T->key_index]);
	}else{
		return T->key_array[T->key_index-1];
	}
}
/*Get T's right-min key*/
static int BTreeGetRightMin(node_t *T){
	if(0 == T->leaf){
		return BTreeGetRightMin(T->child_array[0]);
	}else{
		return T->key_array[0];
	}
}

node_t *delete(int key, btree_t *b)
{
	if(!b->lock){
		if(b->root->key_index == 1){
			node_t *child1 = b->root->child_array[0];
			node_t *child2 = b->root->child_array[1];
			if((!child1) && (!child2)){
				free(b->root);
                return NULL;
			}else{
                if((NODE_ORDER-1 == child1->key_index) && (NODE_ORDER-1 == child2->key_index)){
					//Merge the children and set child1 to the new root.
					merge_children(b->root, 0, child1, child2);
					free(b->root);
					BTreeDeleteNoNone(key, child1);
					return child1;
				}
            }
		}
		BTreeDeleteNoNone(key, b->root);
	}else{
		printf("Tree is locked\n");
	}
	return b->root;
}

int main(int argc, char *argv[])
{
	btree_t *b = create_btree();
    char c[512];
    int t,val;

    scanf("%d",&t);
    while(t--){
        k=0;
        ans = -1;
        scanf("%s",c);
        if(strcmp(c,"add")==0){
            scanf("%d",&val);
            printf("add(%d) = ok\n",val);
            b->root = insert(val,b);
        }
        else if(strcmp(c,"get")==0){
            scanf("%d",&val);
            result_t *rs = search(val, b->root);
            if(rs->found) printf("get(%d) = %d\n",val,val);
            else printf("get(%d) = not found\n",val);
        }
        else if(strcmp(c,"getk")==0){
            scanf("%d",&val);
            find_rank(val,b->root);
            if(ans>=0){
                printf("getk(%d) = %d\n",val,ans);
            }else{
                printf("getk(%d) = not found\n",val);
            }
        }
        else if(strcmp(c,"remove")==0){
            scanf("%d",&val);
            result_t *rs = search(val, b->root);
            if(rs->found){
                printf("remove(%d) = %d\n",val,val);
                b->root = delete(val, b);
                if(!b->root) b = create_btree();
            }else{
                printf("remove(%d) = not found\n",val);
            }
        }
        else if(strcmp(c,"removek")==0){
            scanf("%d",&val);
            find_rank(val,b->root);
            if(ans>=0){
                printf("removek(%d) = %d\n",val,ans);
                result_t *rs = search(ans, b->root);
                if(rs->found){
                    b->root = delete(ans, b);
                    if(!b->root) b = create_btree();
                }
            }else{
                printf("removek(%d) = not found\n",val);
            }
        }
    }
	return 0;
}