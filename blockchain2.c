//gcc blockchain1.c -L/usr/include -lcrypto 
// 1 1 1 2 1 3 1 4 4 2 99 3 5



#include <stdio.h>
#include <openssl/crypto.h>
#include <openssl/sha.h>
#include <string.h>
#include <stdlib.h>

/*for some reason hash is calculated like 3030 instead of 00 (or 303030 instead of 000). still difficulty is present for PoW.  */
static int difficulty = 2; // using difficulty > 2 takes too much time. crashed a few times.

struct block{
    unsigned char prevHash[SHA256_DIGEST_LENGTH], blockHash[SHA256_DIGEST_LENGTH];
    int blockData;
    unsigned long int nonce;
    struct block *link;
}*head;

void addBlock (int);
void verifyChain();
void alterNthBlock (int ,int );
void hackChain();
void hashPrinter(unsigned char * ,int);
int hashCompare (unsigned char *, unsigned char * );
void printAllBlocks();
void printBlock(struct block * );

/*helper : returns block address as unsigned char pointer */
unsigned char* toString(struct block b){
    unsigned char *str = malloc(sizeof(unsigned char *)*sizeof(b));
    memcpy(str,&b,sizeof(b));
    return str;
}
/*helper : print indvidual block */
void printBlock(struct block *b){
    printf("\nBlock Address : %p\n",b);

    printf("Block Data : %d\n",b->blockData);
    printf("Block Nonce : %lu\n",b->nonce);
    printf("Previous Hash : ");
    hashPrinter(b->prevHash,sizeof(b->prevHash));
    printf("Block Hash : ");
    hashPrinter(b->blockHash,sizeof(b->blockHash));
    
}

/*helper : print hash */
void hashPrinter(unsigned char hash[], int length){
    for(int i=0;i<length;i++)
        printf("%02x",hash[i]);
    printf("\n");
}

/*helper : compares two hash strings */
int hashCompare(unsigned char hash1[],unsigned char hash2[]){
    int i;
    for(i=0;i<SHA256_DIGEST_LENGTH;i++){
        if(hash1[i]!=hash2[i])
            return 0;
    }
    return 1;
}

/*helper : calculates the nonce for given difficulty.
DIfficulty = number of prefix zeros in hash */

int checkHash(unsigned char hash[]){
    int i;
    for(i=0;i<difficulty;i++){
        if(hash[i]!='0')
            return 0;
    }
    return 1;
}

unsigned long int calcHash(struct block *block){
    unsigned long int nonce=0;
    printf("\n\nCALCULATING NONCE : THIS MIGHT TAKE A WHILE...\n\n");
    while(nonce < 4294967295){
        block->nonce=nonce;
        SHA256(toString(*block),sizeof(*block),block->blockHash);
        if(checkHash(block->blockHash))
            return nonce;
        nonce++;
    }

}


/*add a block to the chain */
void addBlock(int data){
    struct block *currentBlock,*newBlockPointer, *newBlock;
    int i;
    if(head == NULL){
        head = malloc(sizeof(struct block));
        head->blockData = data;
        currentBlock = head;
        for(i=0;i<SHA256_DIGEST_LENGTH;i++)
            head->prevHash[i]=0;
        head->nonce = calcHash(currentBlock);
        printBlock(head);
        return;
    }

    currentBlock = head;
    while(currentBlock->link)
        currentBlock = currentBlock->link;

    newBlock = malloc(sizeof(struct block));

    newBlock->blockData = data;
    currentBlock->link = newBlock;
    newBlockPointer = newBlock;
    for(i=0;i<SHA256_DIGEST_LENGTH;i++)
        (newBlock->prevHash)[i]=(currentBlock->blockHash)[i];
    //SHA256(toString(*newBlockPointer),sizeof(*newBlockPointer),newBlock->blockHash);        
    newBlock->nonce = calcHash(newBlockPointer);
    printBlock(newBlock);   
}

/*prints chain */
void printAllBlocks(){
    struct block *current = head;
    int count = 1;
    while(current){
        printf("%d. ",count++);
        printBlock(current);
        printf("\n");
        current=current->link;
    }
}
/*verifies whether chain is valid */
void verifyChain(){
    if(head == NULL){
        printf("Chain empty\n");
        return;
    }

    struct block *curr = head->link,*prev = head;
    while(curr){
        if(!hashCompare(curr->prevHash,prev->blockHash)){
            printf("\nBLOCK HASH MISMATCH!\n\n");
            return;
        }
        prev = curr;
        curr = curr->link;
    }        
    printf("CHAIN VERIFIED\n");
}

/*alters nth block and recalculates block hash*/
void alterNthBlock (int n,int data){
    struct block *current = head;
    if(head==NULL){
        printf("Empty List");
        return;
    }
    while(n--){
        if(!current->link && n>0){
            printf("Nth block does not exist");
            return;
        }
        current = current->link;
    }
    printf("\nBefore : \n");printBlock(current);
    current->blockData = data;
    //SHA256(toString(*current),sizeof(*current),current->blockHash);        
    current->nonce = calcHash(current);
    printf("\nAfter : \n");printBlock(current);
}

/* recalulate hash to hack the chain */
void hackChain(){
    int i,count = 1;
    struct block *curr = head->link, *prev = head;
    if(head == NULL || head->link == NULL){
        printf("Chain Empty");
        return;
    }while(curr){
        if(!hashCompare(curr->prevHash,prev->blockHash)){
            while(curr){
                for(i=0;i<SHA256_DIGEST_LENGTH;i++)
                    (curr->prevHash)[i]=(prev->blockHash)[i];
                //SHA256(toString(*curr),sizeof(*curr),curr->blockHash);
                curr->nonce = calcHash(curr);
                prev = curr;
                curr = curr->link;
            }
            printf("\nFIXED\n");
            return;
        }
        prev = curr;
        curr = curr->link;
    }
    
}


void main(){
    int n,c,r,data;
    while(1){
        printf("\nEnter Choice\n1.Add Block\t2.View Blocks\t3.Verify Chain\t4.Alter Chain\t5.Hack Chain\t6.Exit\n");
        scanf("%d",&c);
        switch(c){
            case 1:
                printf("Enter Data : ");
                scanf("%d",&n);
                addBlock(n);
                break;
            case 2:
                printAllBlocks();
                printf("\n");
                break;
            case 3:
                verifyChain();
                break;
            case 4:
                printf("Enter n and data : ");
                scanf("%d%d",&n,&data);
                alterNthBlock(n-1,data);
                break;
            case 5:
                hackChain();
                break;
            case 6:
                return;
            default:
                printf("Wrong choice\n");
                break;
        }
    }
}
