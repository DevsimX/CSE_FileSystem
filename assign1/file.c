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
int file_getblock(struct unixfilesystem *fs, int inumber, int blockNum, void *buf) {// fs , 1 ,0 , char[512]
    struct inode current_inode;
    if(inode_iget(fs,inumber,&current_inode) < 0 || blockNum < 0){
        return -1;
    }
    int numall = inode_getsize(&current_inode);
    if((current_inode.i_mode & ILARG) != 0){
        uint16_t block_tmp[256];
        uint16_t second_block_tmp[256];
        int block_index;
        uint16_t block_number;
        int block_number_int;
        if(blockNum <= 1791){
            block_index = blockNum / 256;
            if(diskimg_readsector(fs->dfd,current_inode.i_addr[block_index],block_tmp) == DISKIMG_SECTOR_SIZE){
                block_number = block_tmp[blockNum-block_index*256];
                block_number_int = block_number;
                if(diskimg_readsector(fs->dfd,block_number_int,buf) == DISKIMG_SECTOR_SIZE){
                    if(numall % 512 == 0)
                        return 512;
                    if(blockNum == numall / 512)
                        return numall % 512;
                    return 512;
                }
            }
            return -1;
        } else{
            if(diskimg_readsector(fs->dfd,current_inode.i_addr[7],block_tmp) == DISKIMG_SECTOR_SIZE){
                block_index = blockNum/256 - 7;
                if(diskimg_readsector(fs->dfd,block_tmp[block_index],second_block_tmp) == DISKIMG_SECTOR_SIZE){
                    block_number = second_block_tmp[blockNum-(block_index+7)*256];
                    block_number_int = block_number;
                    if(diskimg_readsector(fs->dfd,block_number_int,buf) == DISKIMG_SECTOR_SIZE){
                        if(numall % 512 == 0)
                            return 512;
                        if(blockNum == numall / 512)
                            return numall % 512;
                        return 512;
                    }
                }
                return -1;
            }
            return -1;
        }
    } else{
        if(blockNum <= 7){
            if(diskimg_readsector(fs->dfd,current_inode.i_addr[blockNum],buf) == DISKIMG_SECTOR_SIZE){
                if(numall < 512)
                    return numall;
                if(numall % 512 == 0)
                    return 512;
                if(blockNum == numall / 512)
                    return numall % 512;
                return 512;
            }
            return -1;
        }
    }
    return -1;
//    if(diskimg_readsector(fs->dfd,SUPERBLOCK_SECTOR+(inumber-1)/16+1,inodeArray) == DISKIMG_SECTOR_SIZE) {
//
//        if(diskimg_readsector(fs->dfd,current_inode.i_addr[blockNum],buf) == DISKIMG_SECTOR_SIZE){
//            int numall = inode_getsize(&current_inode);
//            if(numall < 512)
//                return numall;
//            if(numall % 512 == 0)
//                return 512;
//            if(blockNum == numall / 512)
//                return numall % 512;
//            return 512;
//        }
//        return -1;
//    }
//    return -1;
}
