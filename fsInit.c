/**************************************************************
* Class:  CSC-415-0#Spring 2022 
* Names:Rohit Devdhar 
* Student IDs:920487201
* GitHub Name:rohitdevdhar
* Group Name:French Fry Academy
* Project: Basic File System
*
* File: fsInit.c
*
* Description: Main driver for file system assignment.
*
* This file is where you will start and initialize your system
*
**************************************************************/


#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>

#include "fsLow.h"
#include "mfs.h"
#include "vcb.h"

word_t *bitmap;
fNode *fnodes;
nameStruct *fnames;
int nextFnode;
int nextFname;



int initFileSystem (uint64_t numberOfBlocks, uint64_t blockSize)
	{
	printf ("Initializing File System with %ld blocks with a block size of %ld\n", numberOfBlocks, blockSize);
	/* TODO: Add any code you need to initialize your file system. */

	uint64_t numRead;

	vcb * readBlock;
	nextFnode = 0;
	nextFname = 0;

	printf("size of vcb: %lu\n", sizeof(vcb));
	// preparing to read our "magic number"
	//readBlock = malloc(numberOfBlocks*blockSize);
	readBlock = malloc(sizeof(vcb));
	//memset(readBlock, 0, numberOfBlocks*blockSize);
	memset(readBlock, 0, sizeof(vcb));
	int numBlocks;
	numBlocks = sizeof(vcb) / BLOCK_SIZE + 1;  
	printf("numBlocks=%d\n",numBlocks);
	numRead =LBAread(readBlock, MAX_BLOCKS, 0);	
	bitmap = readBlock->bitmap;
	fnodes = readBlock->fNodes;
	fnames = readBlock->fileNames; 
	
	printf("MAX number of blocks is: %d\n", MAX_BLOCKS);
	printf("MAX number of words is: %d\n", MAX_WORDS);
	// comparing our "magic number"
	if(readBlock->magic != magicNum){

		// need to initialize our volume control block because our
		// "magic number didn't match the one on the file system
		printf("%s\n", "Need to initialize vcb.");	

		// writing our "magic number"
		readBlock->magic = magicNum;

		// setting our file system block size
		readBlock->blockSize = blockSize;

		// initializing our bitmap 

		int bitmapSize = MAX_WORDS * BITS_PER_WORD / 8 / BLOCK_SIZE;
		printf("bitmap size: %d\n", bitmapSize);

		
		for(int i = 0; i < MAX_BLOCKS; i++){
			clearBit(readBlock->bitmap,(unsigned int) i);
		}
		
		

		// writing our vcb and free space list to the disk
//		LBAwrite(readBlock, bitmapSize + 1, 0);

		// initializing directory structure
/*
		int sods = sizeof(dirStruct);
		rootDir = malloc(50 * (blockSize/sods));
		memset(rootDir, 0, 50 * (blockSize/sods));
		rootDir[0].fileName=malloc(2);
		strcpy(rootDir[0].fileName, "/");	


		// writing our directory structure to disk
		LBAwrite(rootDir, 10, 9);
*/
		for(int i = 0; i < numBlocks; i++){
			setBit(readBlock->bitmap, (unsigned int) i);
		}
		headPtr = makeFreeSpaceLL(readBlock->bitmap);

		struct fs_diriteminfo *rootDirInfo;
		rootDirInfo = malloc(sizeof(struct fs_diriteminfo));
		memset(rootDirInfo, 0, sizeof(struct fs_diriteminfo));
		rootDirInfo->fileType = FT_DIRECTORY;
		strcpy(rootDirInfo->d_name, "/");
		rootDirInfo->d_reclen = 1; 

		fnodes[nextFnode].fileType = FT_DIRECTORY;
		fnodes[nextFnode].permissions = 7;
		fnodes[nextFnode].size = 0;
		fnodes[nextFnode].crTime = 100;
		int *myFsbs;
		myFsbs = getFSBs(headPtr, 1);
		printf("first datablock at:%d\n",myFsbs[0]);
		fnodes[nextFnode].datablocks[0] = myFsbs[0];

		fnames[nextFname].fNode = nextFnode;
		fnames[nextFname].nameLen = 2;
		fnames[nextFname].fileType = FT_DIRECTORY;
		strcpy(fnames[nextFname].name, "/");

		setBit(readBlock->bitmap, myFsbs[0]);

		LBAwrite(readBlock, numBlocks, 0);
		LBAwrite(rootDirInfo, 1, myFsbs[0]);

		nextFnode += 1;
		nextFname += 1;
		
		printf("%s\n", "Initialized");	

	}else{
		printf("%s\n", "Already initialized.");
	}
	headPtr = makeFreeSpaceLL(readBlock->bitmap);
	int *blocks;
	blocks = getFSBs(headPtr, 10);
	printf("blocks = %ls\n", blocks);
	for(int i = 0; i < 10; i++){
		printf("%d", blocks[i]);
	}
	printf("\n");
	return 0;
	}
	
	
void exitFileSystem ()
	{
	printf ("System exiting\n");
	}
