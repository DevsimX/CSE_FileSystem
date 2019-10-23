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
        *inp = inodeArray[inumber - 1 - ((inumber - 1) / 16) * 16];
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
    return 0;
}


/**
 * Computes the size in bytes of the file identified by the given inode
 */
int inode_getsize(struct inode *inp) {
    int low_byte = inp->i_size1;
    int high_byte = inp->i_size0;
    int seconde_half_byte_of_high_byte = high_byte % 16;
    int first_half_byte_of_high_byte = (high_byte - seconde_half_byte_of_high_byte)/16;
    int result = low_byte + (int)pow(16,4)*seconde_half_byte_of_high_byte + (int)pow(16,5)*first_half_byte_of_high_byte;
    return result;
}
