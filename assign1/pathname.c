
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
    strcpy(tmpPathName,pathname);//copy path name.
    struct direntv6 directory;//used to store the directory.

    int dirinumber = 1;
    if(strcmp(pathname,"/") == 0)
        return 1;
    splitPathName = strtok(tmpPathName,"/");
    //Split the pathname.
    if(splitPathName == NULL)
        return -1;//If any error happened, return -1.
    while(splitPathName != NULL){
        //Traverse all the parts of splitPathName.
        if(directory_findname(fs,splitPathName,dirinumber,&directory) == 0){
            dirinumber = directory.d_inumber;//Update dirinumber
        } else
            return -1;//If not found, return -1;
        splitPathName = strtok(NULL,"/");
    }
    return dirinumber;
}
