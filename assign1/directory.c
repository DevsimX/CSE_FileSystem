#include "directory.h"
#include "inode.h"
#include "diskimg.h"
#include "file.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

/**
 * Looks up the specified name (name) in the specified directory (dirinumber).  
 * If found, return the directory entry in space addressed by dirEnt.  Returns 0 
 * on success and something negative on failure. 
 */
int directory_findname(struct unixfilesystem *fs, const char *name, int dirinumber, struct direntv6 *dirEnt) {
    int maxNumEntries = 1000;
    struct direntv6 directories[maxNumEntries];
    struct inode in;
    int err = inode_iget(fs, dirinumber, &in);//get current directory
    if (err < 0) return err;

    if (!(in.i_mode & IALLOC) || ((in.i_mode & IFMT) != IFDIR)) {
        /* Not allocated or not a directory */
        return -1;
    }

    if (maxNumEntries < 1) return -1;
    int size = inode_getsize(&in);//get current directory's size

    assert((size % sizeof(struct direntv6)) == 0);

    int count = 0;
    int numBlocks  = (size + DISKIMG_SECTOR_SIZE - 1) / DISKIMG_SECTOR_SIZE;
    char buf[DISKIMG_SECTOR_SIZE];
    struct direntv6 *dir = (struct direntv6 *) buf;//this step is confusing
    for (int bno = 0; bno < numBlocks; bno++) {
        int bytesLeft, numEntriesInBlock, i;
        bytesLeft = file_getblock(fs, dirinumber,bno,dir);
        if (bytesLeft < 0) {
            fprintf(stderr, "Error reading directory\n");
            return -1;
        }
        numEntriesInBlock = bytesLeft/sizeof(struct direntv6);
        for (i = 0; i <  numEntriesInBlock ; i++) {
            directories[count] = dir[i];
            count++;
        }
    }//Same with the code in GetDirEntries(diskimageaccess.c).
    for(int i = 0 ; i < count ; i++){
        if(strcmp(name,directories[i].d_name) == 0){
            *dirEnt = directories[i];//If found, return the directory entry in space addressed by dirEnt.
            return 0;
        }
    }
    return -1;
}
