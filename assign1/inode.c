#include <stdio.h>
#include <assert.h>
#include <math.h>

#include "inode.h"
#include "diskimg.h"


/**
 * Fetches the specified inode from the filesystem. 
 * Returns 0 on success, -1 on error.  
 */
int inode_iget(struct unixfilesystem *fs, int inumber, struct inode *inp) {
    if(inumber < 1)
        return -1;
    struct inode inodeArray[16];
    if(diskimg_readsector(fs->dfd,SUPERBLOCK_SECTOR+(inumber-1)/16+1,inodeArray) == DISKIMG_SECTOR_SIZE) {
        //Since one block can store 16 inodes, so SUPERBLOCK_SECTOR+(inumber-1)/16+1 represents the real block index that stores inp.
        *inp = inodeArray[inumber - 1 - ((inumber - 1) / 16) * 16];
        //Get the inp by calculating its index in inodeArray.
        return 0;
    }
    return -1;
}


/**
 * Given an index of a file block, retrieves the file's actual block number
 * of from the given inode.
 *
 * Returns the disk block number on success, -1 on error.  
 */
int inode_indexlookup(struct unixfilesystem *fs, struct inode *inp, int blockNum) {
    /*To write comment easily and make comment easy to understand since my english is poor,
     * i'll use block A to replace the block represented by blockNum.
     */
    if(inp->i_mode & ILARG){
        int block_index;
        uint16_t singly_indirect_block[256];
        //array used to store block number whose block stored by singly indirect way.
        uint16_t doubly_indirect_block[256];
        //array used to store block number whose block stored by doubly indirect wat.
        uint16_t block_number;
        //two bytes's block number
        if(blockNum <= 1791){
            /*when block A's block index <= 1791,
             * block A is stored by singly indirect way.
             * The following explains why.
             * The indirect way is that using a block to store 256 block numbers.
             * As for doubly indirect way, i think you'll understand it after you understand singly indirect way.
             * And unix v6 uses 7 blocks for singly indirect and 1 block for doubly indirect.
             * So first 7 blocks store 256x7=1792 blocks.
             * That's why when index <= 1791, i find block number by singly indirect.
             */
            block_index = blockNum / 256;
            //Find out the inp's block index that the block A stores in.
            if(diskimg_readsector(fs->dfd,inp->i_addr[block_index],singly_indirect_block) == DISKIMG_SECTOR_SIZE){
                //As mentioned above, get that inp's block and stores its content in variable singly_indirect_block.
                block_number = singly_indirect_block[blockNum-block_index*256];
                return block_number;
                //Get block A's real block number and return.
            } else
                return -1;
        } else{
            if(diskimg_readsector(fs->dfd,inp->i_addr[7],singly_indirect_block) == DISKIMG_SECTOR_SIZE){
                /*When block A's block index > 1791, it is stored by doubly indirect way.
                 * So it must be in the last block of inp.
                 * So we read inp's last block, and stores its content in variable singly_indirect_block.
                 */
                block_index = blockNum/256 - 7;
                //Find out the singly_indirect_block's index that the block A stores in.
                if(diskimg_readsector(fs->dfd,singly_indirect_block[block_index],doubly_indirect_block) == DISKIMG_SECTOR_SIZE){
                    //As mentioned above, read that singly_indirect_block's block and stores its content in variable in doubly_indirect_block
                    block_number = doubly_indirect_block[blockNum-(block_index+7)*256];
                    return block_number;
                    //Get block A's real block number and return.
                }
                else
                    return -1;
            }
            return -1;
        }
    } else{
        if(blockNum <= 7){
            return inp->i_addr[blockNum];
            //Since block A is stored in direct way. So we can easily find it in inp.
        } else
            return -1;
    }
}


/**
 * Computes the size in bytes of the file identified by the given inode
 */
int inode_getsize(struct inode *inp) {
    return ((inp->i_size0 << 16) | inp->i_size1);
}
