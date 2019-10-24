#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "file.h"
#include "inode.h"
#include "diskimg.h"

/**
 * Fetches the specified file block from the specified inode.
 * Returns the number of valid bytes in the block, -1 on error.
 */
int file_getblock_ILARG(struct unixfilesystem *fs,struct inode currentInode,int blockNum,void *buf,int inodeSize);
int file_getblock_not_ILARG(struct unixfilesystem *fs,struct inode currentInode,int blockNum,void *buf,int inodeSize);
int file_getblock(struct unixfilesystem *fs, int inumber, int blockNum, void *buf) {
    struct inode currentInode;
    if(inode_iget(fs,inumber,&currentInode) < 0 || blockNum < 0){
        //find the inode by its inumber
        return -1;
    }
    int inodeSize = inode_getsize(&currentInode);
    //get the currentInode's size
    if((currentInode.i_mode & ILARG) != 0){
        return file_getblock_ILARG(fs,currentInode,blockNum,buf,inodeSize);
        //if the file uses large addressing algorithm, run this.
    } else{
        return file_getblock_not_ILARG(fs,currentInode,blockNum,buf,inodeSize);
        //if the file doesn't use large addressing algorithm, run this.
    }
}
int file_getblock_ILARG(struct unixfilesystem *fs,struct inode currentInode,int blockNum,void *buf,int inodeSize){
    if(diskimg_readsector(fs->dfd,inode_indexlookup(fs,&currentInode,blockNum),buf) == DISKIMG_SECTOR_SIZE){
        //Get the specified file block.
        if(inodeSize % 512 == 0)
            return 512;
        //If size%512 == 0, it means every block's valid bytes is 512.
        if(blockNum == inodeSize / 512)
            return inodeSize % 512;
        return 512;
        //If size%512 !=0, it means instead of the last block, other block's valid bytes is 512 and the last block's valid bytes is size%512
    } else
        return -1;
}
int file_getblock_not_ILARG(struct unixfilesystem *fs,struct inode currentInode,int blockNum,void *buf,int inodeSize){
    if(diskimg_readsector(fs->dfd,inode_indexlookup(fs,&currentInode,blockNum),buf) == DISKIMG_SECTOR_SIZE){
        if(inodeSize < 512)
            return inodeSize;
        //If size < 512, just return it. Since there is only one block.
        if(inodeSize % 512 == 0)
            return 512;
        if(blockNum == inodeSize / 512)
            return inodeSize % 512;
        //Look above(file_getblock_ILARG), it's same.
        return 512;
    } else
        return -1;
}
