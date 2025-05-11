
#include "pathname.h"
#include "directory.h"
#include "inode.h"
#include "diskimg.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

/**
 * Returns the inode number associated with the specified pathname.  This need only
 * handle absolute paths.  Returns a negative number (-1 is fine) if an error is 
 * encountered.
 */
int pathname_lookup(struct unixfilesystem *fs, const char *pathname) {
    if (pathname[0] != '/') return -1;
    if (pathname[1] == '\0') return 1;  // si es raiz devuelvo

    char pathcopy[strlen(pathname) + 1];
    strcpy(pathcopy, pathname);
    char *token = strtok(pathcopy, "/");

    struct direntv6 dirEnt;
    int dirinumber = 1;

    while (token != NULL) {
        int num = directory_findname(fs, token, dirinumber, &dirEnt);
        if (num != 0) return -1;

        dirinumber = dirEnt.d_inumber;
        token = strtok(NULL, "/");
    }

	return dirinumber;
}
