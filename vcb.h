#include <time.h>
#include <limits.h>
#include <libgen.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define MAX_SIZE 10000384
#define BLOCK_SIZE 512
#define MAX_BLOCKS (MAX_SIZE / BLOCK_SIZE+ 1)

typedef uint32_t word_t;		// unsigned word for basic bitmap use
//#define MAX_WORDS (int) (MAX_BLOCKS / sizeof(word_t)*8) + 1

enum { BITS_PER_WORD = sizeof(word_t)*CHAR_BIT};
#define MAX_WORDS ((MAX_BLOCKS / BITS_PER_WORD) + 1)
#define WORD_OFFSET(b) ((b) /BITS_PER_WORD)
#define BIT_OFFSET(b) ((b) % BITS_PER_WORD)



const int magicNum = 0x175;

// structure for free space block management
typedef struct llFsb{
	int blockNumber;
	struct llFsb *next;
}	llFsb;

typedef struct fNode{
	unsigned char fileType;
	unsigned short permissions;
	unsigned short size;
	unsigned short crTime;
	unsigned short datablocks[20];
}	fNode;


typedef struct nameStruct{
	unsigned short fNode;
	unsigned short nameLen;
	unsigned char fileType;
	char name[255];
}	nameStruct;

typedef struct freeSpace{
	int blockNum;
	struct freeSpace *next;
}	freeSpace;

freeSpace *headPtr;   


// structure for volume control block
typedef struct svcb {
	int magic;
	uint64_t  blockSize;
	word_t bitmap[MAX_WORDS];
	fNode fNodes[10000];
	nameStruct fileNames[10000];
} 	vcb;


/*
// structure for managing directories
typedef struct sDir{
	int iNode;
	char *fileName;
	int parentiNode;
	llUsb * usb;
	time_t creationTime;
	time_t modifyTime;
	int fileType;
} dirStruct;	
*/
	

void setBit(word_t *bitmap, unsigned int n){
	bitmap[WORD_OFFSET(n)] |= ((word_t) 1 << BIT_OFFSET(n));
}

void clearBit(word_t *bitmap, unsigned int n){
	bitmap[WORD_OFFSET(n)] &= ~((word_t) 1 << BIT_OFFSET(n));
}

int getBit(word_t *bitmap, unsigned int n){
	word_t bit = bitmap[WORD_OFFSET(n)] & ((word_t) 1 << BIT_OFFSET(n));
	return bit != 0;
}
	
freeSpace * makeFreeSpaceLL(word_t *bitmap){
	// given bitmap, make a linked list of free blocks for easy access
	
	freeSpace *headPtr;
	freeSpace *currPtr;
	int val; 
	
	int bitmapSize;

	headPtr = malloc(sizeof(freeSpace));
	memset(headPtr, 0, sizeof(freeSpace));
	currPtr = headPtr;
	bitmapSize = sizeof(bitmap) * sizeof(word_t);
	printf("bitmap size is: %d\n", bitmapSize);
	
	for(unsigned int i = 0; i < MAX_BLOCKS; i++){
		val = getBit(bitmap, (unsigned int) i);
		if(val == 0){
			currPtr->blockNum=i;
			currPtr->next=malloc(sizeof(freeSpace));
			memset(currPtr->next, 0, sizeof(freeSpace));
			currPtr = currPtr->next;
		}
	}
	
	return headPtr;
}

int *getFSBs(freeSpace *headPtr, unsigned int n){
	// a routine to get n number of free space blocks
	
	static int fsbs[20];
	freeSpace *currPtr;
	
	if(n > 20){
		n = 20;
	}
		
	currPtr = headPtr;
	for(int i = 0; i < n; i++){
		fsbs[i] = currPtr->blockNum;
		printf("%d:%d\n", i, fsbs[i]);
		currPtr= currPtr->next;
	}

	return fsbs;
}

typedef struct pathLL{
	char *component;
	int countDeep;
	struct pathLL *next;
} pathLL;

pathLL *pathFinder(char *path){
	pathLL *myPath;
	pathLL *pathHead;
	int currCount;
	currCount = 0;
	char *file = basename(path);
	char *dir = dirname(path);
	myPath = malloc(sizeof(pathLL));
	memset(myPath, 0, sizeof(pathLL));
	pathHead = myPath;
	myPath->component = malloc(sizeof(file)+1);
	strcpy(myPath->component, file);
	myPath->countDeep = currCount;
	myPath->next = NULL;
	currCount += 1;
	while (strcmp(dir, "/") != 0) {
		//printf("%s\n", file);
		//printf(%s\n", dir);
		strcpy(path, dir);
		file = basename(path);
		dir = dirname(path);
		myPath = malloc(sizeof(pathLL));
		memset(myPath, 0, sizeof(pathLL));
		myPath->component = malloc(sizeof(file)+1);
		memset(myPath->component, 0, sizeof(file)+1);
		strcpy(myPath->component, file);
		myPath->countDeep = currCount;
		myPath->next = pathHead;
		pathHead = myPath;
		currCount += 1;
	}
	if (strcmp(file, "/") != 0){
		myPath = malloc(sizeof(pathLL));
		memset(myPath, 0, sizeof(pathLL));
		myPath->component = malloc(sizeof(dir)+1);
		memset(myPath->component, 0, sizeof(dir)+1);
		strcpy(myPath->component, dir);
		myPath->countDeep = currCount;
		myPath->next = pathHead;
		pathHead = myPath;
	}
	return pathHead;
}

//vcb * readBlock;


	
