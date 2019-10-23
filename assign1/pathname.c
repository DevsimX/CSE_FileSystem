
#include "pathname.h"
#include "directory.h"
#include "inode.h"
#include "diskimg.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>


/**
 * Returns the inode number associated with the specified pathname.  This need only
 * handle absolute paths.  Returns splitPathName negative number (-1 is fine) if an error is 
 * encountered.
 */
int pathname_lookup(struct unixfilesystem *fs, const char *pathname) {
    char *splitPathName;
    char tmpPathName[MAXLENGTH];
    strcpy(tmpPathName,pathname);
    struct direntv6 direntries[10000];
    int dirinumber = 1;
    if(strcmp(pathname,"/") == 0)
        return 1;
    splitPathName = strtok(tmpPathName,"/");
    while(splitPathName != NULL){
        dirinumber = directory_findname(fs,splitPathName,dirinumber,direntries);
        if(dirinumber == -1)
            return -1;
        splitPathName = strtok(NULL,"/");
    }
    return dirinumber;
}
