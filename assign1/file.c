#include "file.h"
#include "inode.h"
#include "diskimg.h"

/**
 * Fetches the specified file block from the specified inode.
 * Returns the number of valid bytes in the block, -1 on error.
 */
int file_getblock(struct unixfilesystem *fs, int inumber, int blockNum, void *buf) {
    struct inode currentInode;
    int sectorNum;
    if(inode_iget(fs,inumber,&currentInode) < 0 || blockNum < 0){
        //find the inode by its inumber
        return -1;
    }
    int inodeSize = inode_getsize(&currentInode);
    if((sectorNum = inode_indexlookup(fs,&currentInode,blockNum)) < 0) return -1;
    //find the sector number by the inode and block number, through the function: inode_indexlookup()
    if(diskimg_readsector(fs->dfd,sectorNum,buf) == DISKIMG_SECTOR_SIZE){
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
